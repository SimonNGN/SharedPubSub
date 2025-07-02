#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "SharedPubSub.hpp"
#include "FixedString.h"

using namespace std;

namespace py = pybind11;

// Helper wrappers for overloaded/template methods
template<typename T>
void publisher_set_value(shps::Publisher<T>& pub, const T& value) { pub.setValue(value); }
template<typename T>
void publisher_publish(shps::Publisher<T>& pub, const T& value) { pub.publish(value); }
template<typename T>
void publisher_push(shps::Publisher<T>& pub, const T& value) { pub.push(value); }
template<typename T>
void publisher_setValueAndPush(shps::Publisher<T>& pub, const T& value) { pub.setValueAndPush(value); }
template<typename T>
void publisher_publish_on_change(shps::Publisher<T>& pub, const T& value) { pub.publishOnChange(value); }
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
        .def("rawValue", &shps::Publisher<T>::rawValue, py::return_value_policy::reference)
        .def("setValue", &publisher_set_value<T>)
        .def("readValue", &shps::Publisher<T>::readValue)
        .def("publish", &publisher_publish<T>)
        .def("push", &publisher_push<T>)
        .def("setValueAndPush", &publisher_setValueAndPush<T>)
        .def("publishOnChange", &publisher_publish_on_change<T>);

    py::class_<shps::Subscriber<T>>(m, subname)
        .def(py::init<const std::string&, const std::string&, bool>())
        .def("readValue", &subscriber_read_value<T>)
        .def("readWait", &subscriber_read_wait<T>)
        .def("rawValue", &shps::Subscriber<T>::rawValue, py::return_value_policy::reference);
}

// Macro to define both normal and atomic class
#define DECLARE_PUBSUB_NORMAL_AND_ATOMIC(T, name) \
    declare_pubsub<T>(m, "Publisher_" name, "Subscriber_" name); \
    declare_pubsub<atomic<T>>(m, "Publisher_atomic_" name, "Subscriber_atomic_" name);

PYBIND11_MODULE(SharedPubSubPy, m) {
    // Declare custom type
    py::class_<FixedString<2048>>(m, "FixedString2048")
        .def(py::init<>())
        .def(py::init<string>())
        .def("get", &FixedString<2048>::get)
        .def("set", static_cast<void (FixedString<2048>::*)(const string)>(&FixedString<2048>::set))
        .def("size", &FixedString<2048>::size);

    // Base types
    DECLARE_PUBSUB_NORMAL_AND_ATOMIC(bool,      "bool")
    DECLARE_PUBSUB_NORMAL_AND_ATOMIC(int8_t,    "int8")
    DECLARE_PUBSUB_NORMAL_AND_ATOMIC(uint8_t,   "uint8")
    DECLARE_PUBSUB_NORMAL_AND_ATOMIC(int16_t,   "int16")
    DECLARE_PUBSUB_NORMAL_AND_ATOMIC(uint16_t,  "uint16")
    DECLARE_PUBSUB_NORMAL_AND_ATOMIC(int32_t,   "int32")
    DECLARE_PUBSUB_NORMAL_AND_ATOMIC(uint32_t,  "uint32")
    DECLARE_PUBSUB_NORMAL_AND_ATOMIC(int64_t,   "int64")
    DECLARE_PUBSUB_NORMAL_AND_ATOMIC(uint64_t,  "uint64")
    DECLARE_PUBSUB_NORMAL_AND_ATOMIC(float,     "float")
    DECLARE_PUBSUB_NORMAL_AND_ATOMIC(double,    "double")

    // Custom Types
    declare_pubsub<FixedString<2048>>(m, "Publisher_FixedString2048", "Subscriber_FixedString2048");
}
