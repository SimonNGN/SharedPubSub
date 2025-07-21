add_executable(_1_PubSub_publisher publisher.cpp)
add_executable(_1_PubSub_subscriber subscriber.cpp)
target_include_directories(_1_PubSub_publisher PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(_1_PubSub_subscriber PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(_1_PubSub_publisher PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_1_PubSub
    OUTPUT_NAME publisher
)
set_target_properties(_1_PubSub_subscriber PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_1_PubSub
    OUTPUT_NAME subscriber
)