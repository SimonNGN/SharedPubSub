#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "SharedPubSub.hpp"

using namespace std;

// This class is not thread safe, reading in a queue is recommended
template<size_t N>
class FixedString {
    public:
        char data[N] = {'\0'};
        size_t size_ = 0;

    FixedString(){}

    FixedString(string str){
        size_ = snprintf(data,sizeof(data), "%s",str.c_str());
    }

    string get() const {
        return string(data);
    }

    void set(string str){
        size_ = snprintf(data,sizeof(data), "%s",str.c_str());
    }

    size_t size(){
        return size_;
    }

    bool operator==(const FixedString<N>& other) const {
        return strncmp(data, other.data, N) == 0;
    }
    bool operator==(const string& str) const {
        return strncmp(data, str, N) == 0;
    }

    bool operator!=(const FixedString<N>& other) const {
        return strncmp(data, other.data, N) != 0;
    }
    bool operator!=(const string& str) const {
        return strncmp(data, str, N) != 0;
    }

    FixedString<N>& operator=(const FixedString<N>& other) {
        if (this != &other) {
            set(other.get());
        }
        return *this;
    }

    FixedString<N>& operator=(const string& str) {
        if (this != str) {
            set(str);
        }
        return *this;
    }
    ~FixedString() = default;
};

namespace py = pybind11;

// Helper wrappers for overloaded/template methods
template<typename T>
void publisher_set_value(shps::Publisher<T>& pub, T value) { pub.setValue(value); }
template<typename T>
void publisher_publish(shps::Publisher<T>& pub, T value) { pub.publish(value); }
template<typename T>
void publisher_publish_on_change(shps::Publisher<T>& pub, T value) { pub.publishOnChange(value); }
template<typename T>
T subscriber_read_value(shps::Subscriber<T>& sub) { return sub.readValue(); }
template<typename T>
py::object subscriber_read_wait(shps::Subscriber<T>& sub) {
    py::gil_scoped_release release; // Release the GIL here
    auto val = sub.readWait();
    if (val.has_value())
        return py::cast(*val);
    return py::none();
}

template<typename T>
void declare_pubsub(py::module_ &m, const char* pubname, const char* subname) {
    py::class_<shps::Publisher<T>>(m, pubname)
        .def(py::init<const std::string&>())
        .def("setValue", &publisher_set_value<T>)
        .def("publish", &publisher_publish<T>)
        .def("publishOnChange", &publisher_publish_on_change<T>)
        .def("rawValue", &shps::Publisher<T>::rawValue, py::return_value_policy::reference)
        .def("readValue", &shps::Publisher<T>::readValue);

    py::class_<shps::Subscriber<T>>(m, subname)
        .def(py::init<const std::string&, const std::string&, bool>())
        .def("readValue", &subscriber_read_value<T>)
        .def("readWait", &subscriber_read_wait<T>)
        .def("rawValue", &shps::Subscriber<T>::rawValue, py::return_value_policy::reference);
}

PYBIND11_MODULE(SharedPubSubPy, m) {
    declare_pubsub<bool>(m, "PublisherBool", "SubscriberBool");
    declare_pubsub<int8_t>(m, "PublisherInt8", "SubscriberInt8");
    declare_pubsub<uint8_t>(m, "PublisherUInt8", "SubscriberUInt8");
    declare_pubsub<int16_t>(m, "PublisherInt16", "SubscriberInt16");
    declare_pubsub<uint16_t>(m, "PublisherUInt16", "SubscriberUInt16");
    declare_pubsub<int32_t>(m, "PublisherInt32", "SubscriberInt32");
    declare_pubsub<uint32_t>(m, "PublisherUInt32", "SubscriberUInt32");
    declare_pubsub<int64_t>(m, "PublisherInt64", "SubscriberInt64");
    declare_pubsub<uint64_t>(m, "PublisherUInt64", "SubscriberUInt64");
    declare_pubsub<float>(m, "PublisherFloat", "SubscriberFloat");
    declare_pubsub<double>(m, "PublisherDouble", "SubscriberDouble");

    
    py::class_<FixedString<2048>>(m, "FixedString")
        .def(py::init<>())
        .def(py::init<string>())
        .def("get", &FixedString<2048>::get)
        .def("set", &FixedString<2048>::set)
        .def("size", &FixedString<2048>::size);

    declare_pubsub<FixedString<2048>>(m, "PublisherFixedString", "SubscriberFixedString");
}
