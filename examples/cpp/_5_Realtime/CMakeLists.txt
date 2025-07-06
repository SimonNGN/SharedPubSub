add_executable(_5_Realtime_publisher publisher.cpp)
add_executable(_5_Realtime_subscriber subscriber.cpp)
target_include_directories(_5_Realtime_publisher PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(_5_Realtime_subscriber PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(_5_Realtime_publisher PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_5_Realtime
    OUTPUT_NAME publisher
)
set_target_properties(_5_Realtime_subscriber PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_5_Realtime
    OUTPUT_NAME subscriber
)