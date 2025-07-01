#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "SharedPubSub.hpp"

namespace py = pybind11;

// Helper wrappers for overloaded/template methods
void publisher_set_value(shps::Publisher<int>& pub, int value) {
    pub.setValue(value);
}
void publisher_publish(shps::Publisher<int>& pub, int value) {
    pub.publish(value);
}
void publisher_publish_on_change(shps::Publisher<int>& pub, int value) {
    pub.publishOnChange(value);
}

int subscriber_read_value(shps::Subscriber<int>& sub) {
    return sub.readValue();
}
py::object subscriber_read_wait(shps::Subscriber<int>& sub) {
    auto val = sub.readWait();
    if (val.has_value())
        return py::int_(*val);
    return py::none();
}

PYBIND11_MODULE(SharedPubSubPy, m) {
    py::class_<shps::Publisher<int>>(m, "Publisher")
        .def(py::init<const std::string&>())
        .def("setValue", &publisher_set_value)
        .def("publish", &publisher_publish)
        .def("publishOnChange", &publisher_publish_on_change)
        .def("rawValue", &shps::Publisher<int>::rawValue, py::return_value_policy::reference)
        .def("readValue", &shps::Publisher<int>::readValue);

    py::class_<shps::Subscriber<int>>(m, "Subscriber")
        .def(py::init<const std::string&, const std::string&, bool>())
        .def("readValue", &subscriber_read_value)
        .def("readWait", &subscriber_read_wait)
        .def("rawValue", &shps::Subscriber<int>::rawValue, py::return_value_policy::reference);
}