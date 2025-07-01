// This file contains the definitions of the Publisher and Subscriber classes, which are templated for inter-process communication using POSIX shared memory. It includes methods for publishing and subscribing to data, as well as managing shared memory and queues.

#pragma once
#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <fcntl.h>
#include <iostream>
#include <optional>
#include <pthread.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <type_traits>

namespace shps {

template <typename T>
class Publisher;

template <typename T>
class Subscriber;

template <typename T>
class Topic;

template <typename T>
class SharedMemoryManager;

template<typename T>
class NotifiedQueue;

template <class T, size_t N>
class LockFreeQueue;

template<typename T>
struct remove_atomic { using type = T; };

template<typename U>
struct remove_atomic<std::atomic<U>> { using type = U; };

template<typename T>
using remove_atomic_t = typename remove_atomic<T>::type;

template<typename T>
struct is_std_atomic : std::false_type {};

template<typename U>
struct is_std_atomic<std::atomic<U>> : std::true_type {};

template <typename T>
class Publisher : public SharedMemoryManager<T> {
public:
    Publisher(std::string topicName_)
    : topicName(topicName_), SharedMemoryManager<T>() {
        topic = this->openSharedMemoryTopic(topicName_);
        if (!topic) {
            throw std::runtime_error("Failed to open shared memory topic: " + topicName);
        }
        updateValueTemp();
    }

    T* rawValue() {
        return &(topic->value);
    }

    template<typename U>
    void setValue(U&& value) {
        topic->value = std::forward<U>(value);
        updateValueTemp();
    }

    remove_atomic_t<T> readValue() {
        return topic->getValue();
    }

    template<typename U>
    void publish(U&& value) {
        setValue(value);
        updateSubscriberQueues();
        for (int i = 0; i < subscriberQueueCount; ++i) {
            subscriberQueues[i]->pushNotify(value);
        }
    }

    template<typename U>
    void push(U&& value) {
        updateSubscriberQueues();
        for (int i = 0; i < subscriberQueueCount; ++i) {
            subscriberQueues[i]->push(value);
        }
    }

    template<typename U>
    void setValueAndPush(U&& value) {
        setValue(value);
        push(value);
    }

    template<typename U>
    void publishOnChange(U&& value) {
        if (valueTemp != value) {
            publish(value);
        }
    }

    template<typename U>
    void setValueAndNotifyOnChange(U&& value) {
        if (valueTemp != value) {
            setValue(value);
            notifyAll();
        }
    }

    void notifyAll() {
        updateSubscriberQueues();
        for (int i = 0; i < subscriberQueueCount; ++i) {
            subscriberQueues[i]->notify();
        }
    }

    ~Publisher() {
        for (int i = 0; i < subscriberQueueCount; ++i) {
            NotifiedQueue<remove_atomic_t<T>>::closeQueueHandle(subscriberQueues[i]);
        }
        if (topic != nullptr) {
            Topic<T>::closeTopicHandle(topic);
        }
    }

private:
    Topic<T>* topic = nullptr;
    std::string topicName;
    T valueTemp;
    int subscriberQueueCount = 0;
    NotifiedQueue<remove_atomic_t<T>>* subscriberQueues[Topic<T>::subscriberListMax] = {nullptr};

    void updateSubscriberQueues() {
        int listIndex = topic->subscriberListIndex;
        int queueCount = subscriberQueueCount;
        if (queueCount < listIndex) {
            for (int i = subscriberQueueCount; i < topic->subscriberListIndex; ++i) {
                subscriberQueues[i] = this->openSharedQueue(topic->subscriberListName[i]);
                if (subscriberQueues[i] == nullptr) {
                    // Handle error
                }
            }
        }
        subscriberQueueCount = listIndex;
    }

    void updateValueTemp() {
        valueTemp = topic->getValue();
    }
};

template <typename T>
class Subscriber : public SharedMemoryManager<T> {
public:
    Subscriber(std::string topicName_, std::string subscriberName_, bool doSubscribe = false) :
    subscriberName(subscriberName_), topicName(topicName_), SharedMemoryManager<T>() {
        topic = this->openSharedMemoryTopic(topicName_);
        if (topic == nullptr) {
            throw std::runtime_error("Failed to open shared memory topic: " + topicName);
        }
        if (doSubscribe) {
            if (!subscribe()) {
                throw std::runtime_error("Failed to open shared memory space with subscriberName: " + subscriberName);
            }
        }
    }

    bool subscribe() {
        if (notifiedQueue != nullptr) {
            return true;
        }

        notifiedQueue = topic->subscribe(subscriberName);
        if (notifiedQueue == nullptr) {
            return false;
        }

        notifiedQueue->clearQueue();
        return true;
    }

    T* rawValue() {
        return &(topic->value);
    }

    remove_atomic_t<T> readValue() {
        return topic->getValue();
    }

    std::optional<remove_atomic_t<T>> readWait() {
        if (notifiedQueue == nullptr) { return std::nullopt; }
        return notifiedQueue->popWait();
    }

    template <typename Rep, typename Period>
    std::optional<T> readWait(std::chrono::duration<Rep, Period> duration) {
        if (notifiedQueue == nullptr) { return std::nullopt; }
        return notifiedQueue->popWait(duration);
    }

    void waitForNotify() {
        if (notifiedQueue == nullptr) { return; }
        notifiedQueue->waitForNotify();
    }

    template <typename Rep, typename Period>
    void waitForNotify(std::chrono::duration<Rep, Period> duration) {
        if (notifiedQueue == nullptr) { return; }
        notifiedQueue->waitForNotify(duration);
    }

    ~Subscriber() {
        if (notifiedQueue != nullptr) {
            NotifiedQueue<remove_atomic_t<T>>::closeQueueHandle(notifiedQueue);
        }
        if (topic != nullptr) {
            Topic<T>::closeTopicHandle(topic);
        }
    }

private:
    Topic<T>* topic = nullptr;
    std::string topicName;
    std::string subscriberName;
    NotifiedQueue<remove_atomic_t<T>>* notifiedQueue = nullptr;
};

template <typename T>
class Topic {
public:
    static const int subscriberListMax = 2048;
    static const int nameMax = 255;
    T value;
    char subscriberListName[subscriberListMax][nameMax] = {};
    std::atomic<int> subscriberListIndex{0};

    Topic(std::string name) {
        if (name.size() > nameMax) {
            throw std::runtime_error("Topic name must be inferior to " + std::to_string(nameMax));
        }
        snprintf(this->name, sizeof(this->name), "%s", name.c_str());

        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
        pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
        pthread_mutex_init(&m, &attr);
    }

    NotifiedQueue<remove_atomic_t<T>>* subscribe(std::string name) {
        if (subscriberListIndex > subscriberListMax || name.size() >= nameMax) {
            std::cerr << "Subscriber list is full or name is too long." << std::endl;
            return nullptr;
        }

        if (pthread_mutex_lock(&m) == EOWNERDEAD) {
            pthread_mutex_consistent(&m);
        }

        for (int i = 0; i < subscriberListIndex; i++) {
            if (std::string(subscriberListName[i]) == name) {
                NotifiedQueue<remove_atomic_t<T>>* pNotifiedQueue = SharedMemoryManager<T>::openSharedQueue(name);
                pthread_mutex_unlock(&m);
                if (pNotifiedQueue == nullptr) {
                    // Handle error
                }
                return pNotifiedQueue;
            }
        }

        NotifiedQueue<remove_atomic_t<T>>* pNotifiedQueue = SharedMemoryManager<T>::createSharedQueue(name);
        if (pNotifiedQueue == nullptr) {
            pthread_mutex_unlock(&m);
            return nullptr;
        }
        snprintf(subscriberListName[subscriberListIndex], sizeof(subscriberListName[subscriberListIndex]), "%s", name.c_str());
        subscriberListName[subscriberListIndex][nameMax - 1] = '\0';
        subscriberListIndex++;
        pthread_mutex_unlock(&m);
        return pNotifiedQueue;
    }

    remove_atomic_t<T> getValue() {
        if constexpr (is_std_atomic<T>::value) {
            return value.load();
        } else {
            return value;
        }
    }

    static bool closeTopicHandle(Topic* topic) {
        if (topic) {
            if (munmap(topic, sizeof(Topic<T>)) == -1) {
                perror("munmap failed");
                return false;
            }
        }
        return true;
    }

    ~Topic() {}

    Topic(const Topic&) = delete;
    Topic& operator=(const Topic&) = delete;
    Topic(Topic&&) = delete;
    Topic& operator=(Topic&&) = delete;

private:
    char name[nameMax] = {0};
    pthread_mutex_t m;
};

template <typename T>
class SharedMemoryManager {
public:
    SharedMemoryManager() {}

    static NotifiedQueue<remove_atomic_t<T>>* createSharedQueue(std::string name) {
        int shmFd = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
        if (shmFd == -1) {
            perror("shm_open");
            return nullptr;
        }

        int res = ftruncate(shmFd, sizeof(NotifiedQueue<remove_atomic_t<T>>));
        if (res == -1) {
            perror("ftruncate");
            close(shmFd);
            return nullptr;
        }
        void* pNotifiedQueue = mmap(0, sizeof(NotifiedQueue<remove_atomic_t<T>>), PROT_WRITE, MAP_SHARED, shmFd, 0);
        close(shmFd);
        if (pNotifiedQueue == MAP_FAILED) {
            perror("mmap");
            return nullptr;
        }
        NotifiedQueue<remove_atomic_t<T>>* notifiedQueue = new(pNotifiedQueue) NotifiedQueue<remove_atomic_t<T>>;
        return (NotifiedQueue<remove_atomic_t<T>>*)pNotifiedQueue;
    }

    static NotifiedQueue<remove_atomic_t<T>>* openSharedQueue(std::string name, bool initCondition = false) {
        int shmFd = shm_open(name.c_str(), O_RDWR, 0666);
        void* pNotifiedQueue = mmap(0, sizeof(NotifiedQueue<remove_atomic_t<T>>), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
        close(shmFd);
        if (pNotifiedQueue == MAP_FAILED) {
            perror("mmap");
            return nullptr;
        }
        NotifiedQueue<remove_atomic_t<T>>* notifiedQueue = static_cast<NotifiedQueue<remove_atomic_t<T>>*>(pNotifiedQueue);
        if (initCondition) {
            notifiedQueue->init();
        }
        return notifiedQueue;
    }

protected:
    Topic<T>* openSharedMemoryTopic(std::string topicName) {
        int shmFd = shm_open(topicName.c_str(), O_RDWR, 0666);
        Topic<T>* topic = nullptr;
        if (shmFd == -1) {
            shmFd = shm_open(topicName.c_str(), O_CREAT | O_RDWR, 0666);
            if (shmFd == -1) {
                perror("shm_open");
                return nullptr;
            }
            int ret = ftruncate(shmFd, sizeof(Topic<T>));
            if (ret == -1) {
                perror("ftruncate");
                close(shmFd);
                return nullptr;
            }
            void* pTopic = mmap(0, sizeof(Topic<T>), PROT_WRITE, MAP_SHARED, shmFd, 0);
            close(shmFd);
            if (pTopic == MAP_FAILED) {
                perror("mmap");
                return nullptr;
            }
            topic = new(pTopic) Topic<T>(topicName);
        } else {
            void* pTopic = mmap(0, sizeof(Topic<T>), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
            close(shmFd);
            if (pTopic == MAP_FAILED) {
                perror("mmap");
                return nullptr;
            }
            topic = static_cast<Topic<T>*>(pTopic);
        }
        return topic;
    }

    ~SharedMemoryManager() {}
};

template<typename T>
class NotifiedQueue {
public:
    LockFreeQueue<T, 2048> queue;

    NotifiedQueue() {
        init();
    }

    void push(T data) {
        queue.push(data);
    }

    void pushNotify(T data) {
        queue.push(data);
        notify();
    }

    void init() {
        pthread_mutexattr_t mutex_attr;
        pthread_condattr_t cond_attr;
        pthread_mutexattr_init(&mutex_attr);
        pthread_condattr_init(&cond_attr);

        pthread_condattr_setclock(&cond_attr, CLOCK_MONOTONIC);
        pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
        pthread_mutexattr_setrobust(&mutex_attr, PTHREAD_MUTEX_ROBUST);
        pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
        pthread_mutex_init(&mutex, &mutex_attr);
        pthread_cond_init(&condition, &cond_attr);

        pthread_mutexattr_destroy(&mutex_attr);
        pthread_condattr_destroy(&cond_attr);
    }

    std::optional<T> pop() {
        return queue.pop();
    }

    void clearQueue() {
        if (pthread_mutex_lock(&mutex) == EOWNERDEAD) {
            pthread_mutex_consistent(&mutex);
        }
        while (queue.size() > 0) {
            queue.pop();
        }
        pthread_mutex_unlock(&mutex);
    }

    template <typename Rep, typename Period>
    std::optional<T> popWait(std::chrono::duration<Rep, Period> duration) {
        int64_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
        struct timespec ts;

        if (pthread_mutex_lock(&mutex) == EOWNERDEAD) {
            pthread_mutex_consistent(&mutex);
        }

        if (queue.size() > 0) {
            pthread_mutex_unlock(&mutex);
            return queue.pop();
        }

        clock_gettime(CLOCK_MONOTONIC, &ts);
        add_timespec(&ts, nsec);
        if (pthread_cond_timedwait(&condition, &mutex, &ts) == EOWNERDEAD) {
            pthread_mutex_consistent(&mutex);
        }

        pthread_mutex_unlock(&mutex);
        return queue.pop();
    }

    std::optional<T> popWait() {
        if (pthread_mutex_lock(&mutex) == EOWNERDEAD) {
            pthread_mutex_consistent(&mutex);
        }
        if (queue.size() > 0) {
            pthread_mutex_unlock(&mutex);
            return queue.pop();
        }

        if (pthread_cond_wait(&condition, &mutex) == EOWNERDEAD) {
            pthread_mutex_consistent(&mutex);
        }

        pthread_mutex_unlock(&mutex);
        return queue.pop();
    }

    void waitForNotify() {
        if (pthread_mutex_lock(&mutex) == EOWNERDEAD) {
            pthread_mutex_consistent(&mutex);
        }

        if (pthread_cond_wait(&condition, &mutex) == EOWNERDEAD) {
            pthread_mutex_consistent(&mutex);
        }
        pthread_mutex_unlock(&mutex);
        return;
    }

    template <typename Rep, typename Period>
    void waitForNotify(std::chrono::duration<Rep, Period> duration) {
        int64_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
        struct timespec ts;

        clock_gettime(CLOCK_MONOTONIC, &ts);
        add_timespec(&ts, nsec);

        if (pthread_mutex_lock(&mutex) == EOWNERDEAD) {
            pthread_mutex_consistent(&mutex);
        }

        if (pthread_cond_timedwait(&condition, &mutex, &ts) == EOWNERDEAD) {
            pthread_mutex_consistent(&mutex);
        }

        pthread_mutex_unlock(&mutex);
        return;
    }

    void notify() {
        pthread_cond_broadcast(&condition);
    }

    static bool closeQueueHandle(NotifiedQueue<T>* queue) {
        if (queue) {
            if (munmap(queue, sizeof(NotifiedQueue<T>)) == -1) {
                // Handle error
            }
        }
        return true;
    }

    ~NotifiedQueue() {}

private:
    pthread_mutex_t mutex;
    pthread_cond_t condition;

    void add_timespec(struct timespec *ts, long nanoseconds) {
        ts->tv_nsec += nanoseconds;

        if (ts->tv_nsec >= 1000000000) {
            ts->tv_sec += ts->tv_nsec / 1000000000;
            ts->tv_nsec %= 1000000000;
        }
    }
};

template <class T, size_t N>
class LockFreeQueue {
    std::array<T, N> buffer_{};
    std::atomic<size_t> size_{0};
    size_t read_pos_{0};
    size_t write_pos_{0};
    static_assert(std::atomic<size_t>::is_always_lock_free);

private:
    bool do_push(auto&& t) {
        if (size_.load() == N) {
            return false;
        }
        buffer_[write_pos_] = std::forward<decltype(t)>(t);
        write_pos_ = (write_pos_ + 1) % N;
        size_++;
        return true;
    }

public:
    LockFreeQueue() {}
    bool push(T&& t) {
        return do_push(std::move(t));
    }

    bool push(const T& t) {
        return do_push(t);
    }

    auto pop() -> std::optional<T> {
        auto val = std::optional<T>{};
        if (size_.load() > 0) {
            val = std::move(buffer_[read_pos_]);
            read_pos_ = (read_pos_ + 1) % N;
            size_--;
        }
        return val;
    }

    auto size() const noexcept { return size_.load(); }
};

} // namespace shps