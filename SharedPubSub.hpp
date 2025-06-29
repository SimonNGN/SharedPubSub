/*
    Author: Simon Nguyen
    License: MIT
    Repository: https://github.com/SimonNGN/SharedPubSub
    Created: 2025

    Shared Memory Publisher-Subscriber Library

    Provides Publisher and Subscriber classes for inter-process communication
    using POSIX shared memory and lock-free queues with notification support.

    Subscribers can either:
    - Receive values through a queue with notification (for event-driven or ordered consumption).
    - Directly access the latest value in shared memory (for low-latency polling)

    License at the bottom of the file.
*/

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

/*
    Main functions to use the library :

    Publisher
    ----------
    Publisher(std::string topicName_)
    rawValue() -> T*
    setValue()
    readValue() -> remove_atomic_t<T>
    publish()
    publishOnChange()
    setValueAndNotifyOnChange()
    notifyAll()

    Subscriber
    ----------
    Subscriber(std::string topicName_,std::string subscriberName_,bool doSubscribe = false)
    subscribe()
    readValue() -> remove_atomic_t<T>
    readWait() -> std::optional<remove_atomic_t<T>> 
    readWait(duration) -> std::optional<remove_atomic_t<T>> 
    waitForNotify()
    waitForNotify(duration)

*/

// Forward Declaration of all classes.  //
// From higher level to lower level     //
namespace shps{

template <typename T>
class Publisher;                // To set a value and/or publish it on a topic to notify subscribers

template <typename T>
class Subscriber;               // To read a value and/or subscribe to a topic and get notified.

template <typename T>
class Topic;                    // Shared resource by the publisher and subscribers

template <typename T>
class SharedMemoryManager;      // Utility class to interact with posix shared memory functions

template<typename T>            // Queue with the ability to notify/get notified
class NotifiedQueue;

template <class T, size_t N>    // Lock free Queue
class LockFreeQueue;

template<typename T>
struct remove_atomic { using type = T; };                   // Utility type 1 to deduce non-atomic

template<typename U>
struct remove_atomic<std::atomic<U>> { using type = U; };   // Utility type 2 to deduce non-atomic

template<typename T>
using remove_atomic_t = typename remove_atomic<T>::type;    // Utility type 3 to deduce non-atomic

/*
    Publisher
    -----------
    This class is the main class for someone that wants to share data to subscribers.
    It opens a share memory space and creates a `Topic` object in it.
    It can directly set the value of that topic, set and notify subscribers, set and notify on change,
    or simply notify subscribers.
    The `Topic` object can be opened by subscribers to either read the value directly,
    or consume published value in a queue.
*/
template <typename T>
class Publisher : public SharedMemoryManager<T> {
    public:

        // Constructor, open/create shared memory space with a Topic in it.
        // Throws runtime_error when shared memory does not work.
        Publisher(std::string topicName_)
        : topicName(topicName_),SharedMemoryManager<T>(){
            topic = this->openSharedMemoryTopic(topicName_);
            if(!topic){
                throw std::runtime_error("Failed to open shared memory topic: " + topicName);
            }
            updateValueTemp();
        };

        // returns raw value pointer to use it wherever someone wants.
        T* rawValue(){
            return &(topic->value);
        }

        // Set the topic's value.
        template<typename U>
        void setValue(U&& value){
            topic->value = std::forward<U>(value);
            updateValueTemp();
        };

        // return a copy of the current stored value
        remove_atomic_t<T> readValue(){
            return topic->getValue();
        }

        // Set the topic's value and push it into each subscriber's queue
        // Throws runtime_error if there's a new subscriber and it fails to open the the shared memory
        template<typename U>
        void publish(U&& value){
            setValue(value);
            updateSubscriberQueues();
            for(int i=0;i<subscriberQueueCount;++i){
                subscriberQueues[i]->pushNotify(value);
            }
        };

        // Set the topic's value and push it into subsciber's queue if changed
        template<typename U>
        void publishOnChange(U&& value){
            if(valueTemp!=value){
                publish(value);
            }
        };

        // Set the topic's value and notify subscriber's on change
        template<typename U>
        void setValueAndNotifyOnChange(U&& value){
            if(valueTemp!=value){
                setValue(value);
                notifyAll();
            }
        };
        
        // Notify all subscribers without setting the value.
        void notifyAll(){
            updateSubscriberQueues();
            for(int i=0;i<subscriberQueueCount;++i){
                subscriberQueues[i]->notify();
            }
        };

        // Destructor. Simply close the queue handle in shared memory, 
        // purposefully leaving all data in it.
        ~Publisher(){
            for(int i=0;i<subscriberQueueCount;++i){
                NotifiedQueue<remove_atomic_t<T>>::closeQueueHandle(subscriberQueues[i]);
            }
            if(topic!=nullptr){
                Topic<T>::closeTopicHandle(topic);
            }
        }

    private:
        Topic<T>* topic = nullptr;
        std::string topicName;
        T valueTemp;
        int subscriberQueueCount = 0;
        NotifiedQueue<remove_atomic_t<T>>* subscriberQueues[Topic<T>::subscriberListMax] = {nullptr};

        // Check the subscriber's list in the Topic object and update the publisher's list.
        void updateSubscriberQueues(){
            int listIndex = topic->subscriberListIndex;
            int queueCount = subscriberQueueCount;
            if(queueCount<listIndex){
                for(int i=subscriberQueueCount;i<topic->subscriberListIndex;++i){
                    subscriberQueues[i] = this->openSharedQueue(topic->subscriberListName[i]);
                    if(subscriberQueues[i]==nullptr){
                        throw std::runtime_error("Failed to open shared memory space for new subscriber");
                    }
                }
            }
            subscriberQueueCount = listIndex;
        }

        // Update the temporary value to monitor change for publishOnChange() and setValueAndNotifyOnChange()
        void updateValueTemp(){
            valueTemp = topic->getValue();
        }
};

/*
    Subscriber
    -----------
    This class is the main class for someone that wants to read data from the publisher.
    It opens a share memory space and opens a `Topic` object in it.
    It can directly read the value, wait for notification, or wait and read.
    Optionally, it can open a Queue where data will be received in order.
*/

template <typename T>
class Subscriber : public SharedMemoryManager<T> {
    public:

        // Constructor, open/create shared memory space with a Topic in it.
        // If doSubscribe = True, opens a queue and clears the queue to remove any previous data.
        // Throws runtime_error when shared memory does not work.
        Subscriber(std::string topicName_,std::string subscriberName_,bool doSubscribe = false) : 
        subscriberName(subscriberName_),topicName(topicName_),SharedMemoryManager<T>(){
            topic = this->openSharedMemoryTopic(topicName_);
            if(topic == nullptr){
                throw std::runtime_error("Failed to open shared memory topic: " + topicName);
            }
            if(doSubscribe){
                if(!subscribe()){
                    throw std::runtime_error("Failed to open shared memory space with subscriberName: " + subscriberName);
                }
            }
        };

        // Subscribes to a topic by opening a queue in shared memory
        // Returns false when opening the queue fails
        bool subscribe(){
            if(notifiedQueue!=nullptr){
                return true;
            }

            notifiedQueue = topic->subscribe(subscriberName);
            if(notifiedQueue == nullptr){
                return false;
            }

            notifiedQueue->clearQueue();
            return true;
        }

        // return a copy of the current stored value
        remove_atomic_t<T> readValue(){
            return topic->getValue();
        }  

        // Wait indefinitely for a new value signal and pop the latest value
        // Returns nullopt if no value was in the queue.
        std::optional<remove_atomic_t<T>> readWait(){
            return notifiedQueue->popWait();
        }

        // Wait for a new value signal for a set amout of time and pop the latest value.
        // Will pop on signal, or on timeout.
        // (Ex. time value : 1s,500ms,500us,500ns)
        // Returns nullopt if no value was in the queue.
        template <typename Rep, typename Period>
        std::optional<T> readWait(std::chrono::duration<Rep, Period> duration){
            return notifiedQueue->popWait(duration);
        }

        // Wait indefinitely for a notification
        void waitForNotify(){
            notifiedQueue->waitForNotify();
        }

        // Wait for a signal for a set amount of time (Ex. Value : 1s,500ms,500us,500ns)
        template <typename Rep, typename Period>
        void waitForNotify(std::chrono::duration<Rep, Period> duration){
            notifiedQueue->waitForNotify(duration);
        }

        // Destructor. Simply close the queue handle in shared memory, 
        // purposefully leaving all data in it.
        ~Subscriber(){
            if(notifiedQueue!=nullptr){
                NotifiedQueue<remove_atomic_t<T>>::closeQueueHandle(notifiedQueue);
            }
            if(topic!=nullptr){
                Topic<T>::closeTopicHandle(topic);
            }
        }

    private:
        Topic<T>* topic = nullptr;
        std::string topicName;
        std::string subscriberName;
        NotifiedQueue<remove_atomic_t<T>>* notifiedQueue = nullptr;
};
/*
    Topic
    -----------
    This class contains the data that is shared by the publisher,
    and queues created by subscribers.
*/
template <typename T>
class Topic{

    public:
        static const int subscriberListMax = 2048;  // Arbitrary number
        static const int nameMax = 255;             // Arbitrary number
        T value;
        char subscriberListName[subscriberListMax][nameMax] = {};
        std::atomic<int> subscriberListIndex{0};

        // Constructor
        // Throws if name size is greater than nameMax
        Topic(std::string name){

            if(name.size()>nameMax){
                throw std::runtime_error("Topic name must be inferior to " + std::to_string(nameMax));
            }
            snprintf(this->name,sizeof(this->name),"%s",name.c_str()); 

            // Initialize mutex
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);
            pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED); // Enable inter-process sharing
            pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
            pthread_mutex_init(&m, &attr);
        }

        // Subscribes to the topic by creating a NotifiedQueue in shared memory
        // where the topic can add to the queue or simply notify for a change in value
        NotifiedQueue<remove_atomic_t<T>>* subscribe(std::string name){
            // Handle name size
            if(subscriberListIndex>subscriberListMax || name.size()>=nameMax){
                std::cerr << "Subscriber list is full or name is too long." << std::endl;
                return nullptr;
            }

            // Take a mutex to add to the list of subscribers
            if(pthread_mutex_lock(&m)==EOWNERDEAD){ 
                // If a process dies with the mutex, the other one that gets the mutex will receive EOWNERDEAD
                // TODO : Handle this? might be a catastrophic failure
                pthread_mutex_consistent(&m);
            }

            // Verify if the subscriber's name is already in the list of subscribers
            // If so, return a pointer to it
            for(int i=0;i<subscriberListIndex;i++){
                if(std::string(subscriberListName[i]) == name){
                    NotifiedQueue<remove_atomic_t<T>>* pNotifiedQueue = SharedMemoryManager<T>::openSharedQueue(name);
                    pthread_mutex_unlock(&m);
                    if(pNotifiedQueue==nullptr){
                        return nullptr;
                    }
                    return pNotifiedQueue;
                }
            }
            
            // If the subscriber is new, add it to the subscriber's list
            NotifiedQueue<remove_atomic_t<T>>* pNotifiedQueue = SharedMemoryManager<T>::createSharedQueue(name);
            if(pNotifiedQueue==nullptr){
                pthread_mutex_unlock(&m);
                return nullptr;
            }
            snprintf(subscriberListName[subscriberListIndex],sizeof(subscriberListName[subscriberListIndex]),"%s",name.c_str());
            subscriberListName[subscriberListIndex][nameMax-1] = '\0';
            subscriberListIndex++;
            pthread_mutex_unlock(&m);
            return pNotifiedQueue;
        }

        // Read the current stored value
        remove_atomic_t<T> getValue(){
            // If T is atomic, else do normal. Compile-time if/else.
            if constexpr (std::is_same_v<T, std::atomic<typename T::value_type>>) { 
                return value.load();
            } else {
                return value;
            }
        }

        // Unmap queue's shared memory for the current process
        static bool closeTopicHandle(Topic* topic) {
            if (topic) {
                if (munmap(topic, sizeof(Topic<T>)) == -1) {
                    perror("munmap failed");
                    return false;
                }
            }
            return true;
        }

        ~Topic(){}; // Purposefully empty destructor

        // Delete copy/move/assign
        Topic(const Topic&) = delete;
        Topic& operator=(const Topic&) = delete;
        Topic(Topic&&) = delete;
        Topic& operator=(Topic&&) = delete;

    private:
        char name[nameMax] = {0};
        pthread_mutex_t m;
        
};


/*
    SharedMemoryManager
    -----------
    Utility class to handle shared memo
*/
template <typename T>
class SharedMemoryManager{
    public:
        SharedMemoryManager(){
        }

        // Creates a NotifiedQueue in shared memory using Posix SHM
        // Returns a pointer of the queue
        // If the desired data object is atomic typed, it creates a queue of the non atomic version
        static NotifiedQueue<remove_atomic_t<T>>* createSharedQueue(std::string name){
            // Open shared memory space
            int shmFd = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666); // create the shared memory object
            if(shmFd == -1) {
                perror("shm_open");
                return nullptr;
            }

            // Configure the size of the shared memory object
            int res = ftruncate(shmFd, sizeof(NotifiedQueue<remove_atomic_t<T>>));
            if(res == -1) {
                perror("ftruncate");
                close(shmFd);
                return nullptr;
            }
            void* pNotifiedQueue = mmap(0, sizeof(NotifiedQueue<remove_atomic_t<T>>), PROT_WRITE, MAP_SHARED, shmFd, 0); // memory map the shared memory object
            close(shmFd);
            if (pNotifiedQueue == MAP_FAILED) {
                perror("mmap");
                return nullptr;
            }
            NotifiedQueue<remove_atomic_t<T>>* notifiedQueue = new(pNotifiedQueue)NotifiedQueue<remove_atomic_t<T>>; // Create a Queue in the shared memory space
            return (NotifiedQueue<remove_atomic_t<T>>*)pNotifiedQueue;
        };

        // Opens a posix shared memory space and interpret it as a NotifiedQueue
        // Returns a pointer of the queue
        static NotifiedQueue<remove_atomic_t<T>>* openSharedQueue(std::string name,bool initCondition = false){
            int shmFd = shm_open(name.c_str(), O_RDWR, 0666);
            void* pNotifiedQueue = mmap(0, sizeof(NotifiedQueue<remove_atomic_t<T>>), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
            close(shmFd);
            if (pNotifiedQueue == MAP_FAILED) {
                perror("mmap");
                return nullptr;
            }
            NotifiedQueue<remove_atomic_t<T>>* notifiedQueue = static_cast<NotifiedQueue<remove_atomic_t<T>>*>(pNotifiedQueue);
            if(initCondition){
                // Initialize the condition variable to make it valid
                // if it was held previously
                notifiedQueue->init();
            }
            return notifiedQueue; 
        }

    protected:

        // Opens a posix shared memory space and interpret it as a Topic
        Topic<T>* openSharedMemoryTopic(std::string topicName){
            // Try to open the shared memory
            int shmFd = shm_open(topicName.c_str(), O_RDWR, 0666);
            Topic<T>* topic = nullptr;
            // If it fails, it means it does not exists.
            // Create it
            if(shmFd==-1){
                // Open shared memory space
                shmFd = shm_open(topicName.c_str(), O_CREAT | O_RDWR, 0666);
                if(shmFd == -1){
                    perror("shm_open");
                    return nullptr;
                }
                // Configure the size of the shared memory object
                int ret = ftruncate(shmFd, sizeof(Topic<T>));
                if(ret == -1) {
                    perror("ftruncate");
                    close(shmFd);
                    return nullptr;
                }
                void* pTopic = mmap(0, sizeof(Topic<T>), PROT_WRITE, MAP_SHARED, shmFd, 0); // memory map the shared memory object
                close(shmFd);
                if (pTopic == MAP_FAILED) {
                    perror("mmap");
                    return nullptr;
                }
                topic = new(pTopic)Topic<T>(topicName); // Create a Queue in the shared memory space
            }
            // If it succeeds, map the memory to the object
            else{
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

        ~SharedMemoryManager(){} //Purposefully empty destructor
};

/*
    NotifiedQueue
    -----------
    Contains a LockFreeQueue and condition_variable, and everything to handle them.
*/
template<typename T>
class NotifiedQueue{
    static_assert(std::is_trivially_copyable<T>::value, "The queue data type must be trivially copyable");

    public:
        LockFreeQueue<T,2048> queue; // size 2048 is arbitrary

        NotifiedQueue(){
            init();
        }

        // Add data to the queue
        void push(T data){
            queue.push(data);
        };
        
        // Add data to the queue and notify
        void pushNotify(T data){
            queue.push(data);
            notify();
        };

        // Initialize the mutex and condition variable
        void init(){
            // Create and initialize attributes
            pthread_mutexattr_t mutex_attr;
            pthread_condattr_t cond_attr;
            pthread_mutexattr_init(&mutex_attr);
            pthread_condattr_init(&cond_attr);
            
            // Set attributes
            pthread_condattr_setclock(&cond_attr, CLOCK_MONOTONIC);             // Set clock type for the condition wait timeout
            pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);  // Makes the mutex shareable
            pthread_mutexattr_setrobust(&mutex_attr, PTHREAD_MUTEX_ROBUST);     // Makes it so that we can recover from dropped mutex
            pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);    // Makes the condition variable sharable
            pthread_mutex_init(&mutex, &mutex_attr);                            // Initialize/reset the mutex
            pthread_cond_init(&condition, &cond_attr);                          // Initialize/reset the condition variable

            // Clean the attributes
            pthread_mutexattr_destroy(&mutex_attr);
            pthread_condattr_destroy(&cond_attr);
        }

        // Pops one value
        // Returns nullopt if no value in the queue
        std::optional<T> pop(){
            return queue.pop();
        };

        // Clear the queue of all values
        void clearQueue(){
            if (pthread_mutex_lock(&mutex) == EOWNERDEAD) {
                // lock was lost by the last process. flush the queue.
                
                pthread_mutex_consistent(&mutex);
            }
            while(queue.size()>0){
                queue.pop();
            }
            pthread_mutex_unlock(&mutex);
        }

        // Pop a value if something is in the queue,
        // or wait for a new signal for a set amout of time and pop the latest value.
        // Will pop on signal, or on timeout.
        // (Ex. time value : 1s,500ms,500us,500ns)
        // Returns nullopt if no value was in the queue.
        template <typename Rep, typename Period>
        std::optional<T> popWait(std::chrono::duration<Rep, Period> duration){

            int64_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
            struct timespec ts;

            if (pthread_mutex_lock(&mutex) == EOWNERDEAD) {
                // lock was lost by the last process. flush the queue.
                while(queue.size()>0){
                    queue.pop();
                }
                pthread_mutex_consistent(&mutex);
            }

            if(queue.size()>0){
                pthread_mutex_unlock(&mutex);
                return queue.pop();
            }

            clock_gettime(CLOCK_MONOTONIC, &ts);
            add_timespec(&ts, nsec);
            if(pthread_cond_timedwait(&condition,&mutex,&ts) == EOWNERDEAD){
                // Unlikely to end here because of previous check
                pthread_mutex_consistent(&mutex);
            }
            
            pthread_mutex_unlock(&mutex);
            return queue.pop();
        };

        // Pop a value if something is in the queue, or wait indefinitely for a new value signal 
        // and pop the latest value.
        // Returns nullopt if no value was in the queue.
        std::optional<T> popWait(){
            if (pthread_mutex_lock(&mutex) == EOWNERDEAD) {
                // lock was lost by the last process. flush the queue.
                while(queue.size()>0){
                    queue.pop();
                }
                pthread_mutex_consistent(&mutex);
            }
            // If the queue has something in it, pop the value without waiting
            if(queue.size()>0){
                pthread_mutex_unlock(&mutex);
                return queue.pop();
            }

            if(pthread_cond_wait(&condition,&mutex) == EOWNERDEAD){
                // Unlikely to end here because of previous check
                pthread_mutex_consistent(&mutex);
            }
            
            pthread_mutex_unlock(&mutex);
            return queue.pop();
        };

        // Wait indefinitely for a new signal
        void waitForNotify(){

            // Make the mutex consistent if previous thread died while holding it.
            if (pthread_mutex_lock(&mutex) == EOWNERDEAD) {
                pthread_mutex_consistent(&mutex);
            }

            // Wait for notify
            if(pthread_cond_wait(&condition,&mutex) == EOWNERDEAD){
                // Unlikely to end here because of previous check
                pthread_mutex_consistent(&mutex);
            }
            pthread_mutex_unlock(&mutex);
            return;
        };

        // Wait for a new signal for a set amout of time.
        // Will return on signal or timeout.
        template <typename Rep, typename Period>
        void waitForNotify(std::chrono::duration<Rep, Period> duration){

            int64_t nsec = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
            struct timespec ts;

            clock_gettime(CLOCK_MONOTONIC, &ts);
            add_timespec(&ts, nsec);

            // Make the mutex consistent if previous thread died while holding it.
            if (pthread_mutex_lock(&mutex) == EOWNERDEAD) {
                pthread_mutex_consistent(&mutex);
            }

            // Wait for notify
            if(pthread_cond_timedwait(&condition,&mutex,&ts) == EOWNERDEAD){
                // Unlikely to end here because of previous check
                pthread_mutex_consistent(&mutex);
            }
            
            pthread_mutex_unlock(&mutex);
            return;
        };

        void notify(){
            // Purposefully not holding the mutex
            pthread_cond_broadcast(&condition);
        };

        // Unmap queue's shared memory for the current process
        static bool closeQueueHandle(NotifiedQueue<T>* queue) {
            if (queue) {
                if (munmap(queue, sizeof(NotifiedQueue<T>)) == -1) {
                    perror("munmap failed");
                    return false;
                }
            }
            return true;
        }

        // Destroys the mutex and condition_variable
        void clean() {
            pthread_cond_destroy(&condition);
            pthread_mutex_destroy(&mutex);
        }

        // Destructor
        // Purposefully empty
        ~NotifiedQueue(){
        };

    private:
        pthread_mutex_t mutex;
        pthread_cond_t condition;

        void add_timespec(struct timespec *ts, long nanoseconds) {
            ts->tv_nsec += nanoseconds;

            // Handle overflow of nanoseconds
            if (ts->tv_nsec >= 1000000000) {
                ts->tv_sec += ts->tv_nsec / 1000000000;
                ts->tv_nsec %= 1000000000;
            }
        }

};

// Snippet from "C++ High Performance" book's Lock-free queue
template <class T, size_t N>
class LockFreeQueue{
    std::array<T,N> buffer_{};
    std::atomic<size_t> size_{0};
    size_t read_pos_{0};
    size_t write_pos_{0};
    static_assert(std::atomic<size_t>::is_always_lock_free);

    private:
        bool do_push(auto&& t){
            if(size_.load()==N){
                return false;
            }
            buffer_[write_pos_] = std::forward<decltype(t)>(t);
            write_pos_ = (write_pos_+1) % N;
            size_++;
            return true;
        }

    public:
        LockFreeQueue(){}
        bool push(T&& t){ 
            return do_push(std::move(t));
        }

        bool push(const T& t){ 
            return do_push(t);
        }

        auto pop() -> std::optional<T> {
            auto val = std::optional<T>{};
            if(size_.load()>0){
                val = std::move(buffer_[read_pos_]);
                read_pos_ = (read_pos_ + 1) % N;
                size_--;
            }
            return val;
        }

        auto size() const noexcept { return size_.load();}
};
} // namespace

/*

MIT License

Copyright (c) 2025 Simon Nguyen

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
