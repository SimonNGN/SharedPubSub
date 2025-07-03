add_executable(_8_FixedString_publisher publisher.cpp)
add_executable(_8_FixedString_subscriber subscriber.cpp)
target_include_directories(_8_FixedString_publisher PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(_8_FixedString_subscriber PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(_8_FixedString_publisher PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_8_FixedString
    OUTPUT_NAME publisher
)
set_target_properties(_8_FixedString_subscriber PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_8_FixedString
    OUTPUT_NAME subscriber
)