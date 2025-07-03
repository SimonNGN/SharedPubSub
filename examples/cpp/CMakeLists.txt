cmake_minimum_required(VERSION 3.10)
project(SharedPubSub)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/../../util)
include_directories(${CMAKE_SOURCE_DIR}/../..)

# Add all subdirectories in ./examples that have a CMakeLists.txt
file(GLOB EXAMPLES_SUBDIRS RELATIVE ${CMAKE_SOURCE_DIR}/ ${CMAKE_SOURCE_DIR}/*)

foreach(subdir ${EXAMPLES_SUBDIRS})
    if(EXISTS "${CMAKE_SOURCE_DIR}/${subdir}/CMakeLists.txt")
        add_subdirectory(${subdir})
    endif()
endforeach()