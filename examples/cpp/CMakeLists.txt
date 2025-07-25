cmake_minimum_required(VERSION 3.10)
project(SharedPubSub)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/../../util)
include_directories(${CMAKE_SOURCE_DIR}/../..)

file(GLOB EXAMPLE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/_*")

foreach(EXAMPLE_DIR ${EXAMPLE_DIRS})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_DIR} NAME)
    
    if(EXISTS "${EXAMPLE_DIR}/publisher.cpp")
        add_executable(${EXAMPLE_NAME}_publisher ${EXAMPLE_DIR}/publisher.cpp)
        
        target_include_directories(${EXAMPLE_NAME}_publisher PRIVATE ${CMAKE_SOURCE_DIR})
        
        set_target_properties(${EXAMPLE_NAME}_publisher PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${EXAMPLE_NAME}
            OUTPUT_NAME publisher
        )
    endif()

    if(EXISTS "${EXAMPLE_DIR}/subscriber.cpp")
        add_executable(${EXAMPLE_NAME}_subscriber ${EXAMPLE_DIR}/subscriber.cpp)
        target_include_directories(${EXAMPLE_NAME}_subscriber PRIVATE ${CMAKE_SOURCE_DIR})
        set_target_properties(${EXAMPLE_NAME}_subscriber PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${EXAMPLE_NAME}
            OUTPUT_NAME subscriber
        )
    endif()
endforeach()