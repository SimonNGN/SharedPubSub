cmake_minimum_required(VERSION 3.10)
project(SharedPubSub)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Adds all .cpp files
add_executable (publisher src/publisher.cpp)
add_executable (subscriber src/subscriber.cpp)

# Add include directory
target_include_directories(publisher PRIVATE ./)
target_include_directories(subscriber PRIVATE ./)