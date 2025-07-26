add_executable(_7_Demo_publisher publisher.cpp)
target_include_directories(_7_Demo_publisher PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(_7_Demo_publisher PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_7_Demo
    OUTPUT_NAME publisher
)
