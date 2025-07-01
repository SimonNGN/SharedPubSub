cmake_minimum_required(VERSION 3.14)
project(SharedPubSubPy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PythonLibs REQUIRED)
find_package(pybind11 REQUIRED)

include_directories(${PYTHON_INCLUDE_DIRS})

pybind11_add_module(SharedPubSubPy src/pybind_module.cpp)

target_link_libraries(SharedPubSubPy PRIVATE ${PYTHON_LIBRARIES})