add_executable(_3_Object_publisher publisher.cpp)
add_executable(_3_Object_subscriber subscriber.cpp)
target_include_directories(_3_Object_publisher PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(_3_Object_subscriber PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(_3_Object_publisher PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_3_Object
    OUTPUT_NAME publisher
)
set_target_properties(_3_Object_subscriber PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_3_Object
    OUTPUT_NAME subscriber
)