cmake_minimum_required(VERSION 3.14)
project(SharedPubSubPy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Python REQUIRED COMPONENTS Interpreter Development)
find_package(pybind11 REQUIRED)

include_directories(${Python_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/../)
include_directories(${CMAKE_SOURCE_DIR}/../util)

pybind11_add_module(SharedPubSubPy src/pybind_module.cpp)

target_link_libraries(SharedPubSubPy PRIVATE ${Python_LIBRARIES})