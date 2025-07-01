#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "SharedPubSub.hpp"

struct SharedString1024 {
    char data[1024];

    SharedString1024() { data[0] = '\0'; }

    SharedString1024(const char* src) {
        strncpy(data, src, 1023);
        data[1023] = '\0';
    }

    bool operator==(const SharedString1024& other) const {
        return strncmp(data, other.data, 1024) == 0;
    }
    bool operator!=(const SharedString1024& other) const {
        return !(*this == other);
    }
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

    
    py::class_<SharedString1024>(m, "SharedString1024")
        .def(py::init<>())
        .def(py::init<const char*>())
        .def_property("value",
            [](const SharedString1024& s) { return std::string(s.data); },
            [](SharedString1024& s, const std::string& v) {
                strncpy(s.data, v.c_str(), 1023);
                s.data[1023] = '\0';
            }
        );

    declare_pubsub<SharedString1024>(m, "PublisherString1024", "SubscriberString1024");
}
