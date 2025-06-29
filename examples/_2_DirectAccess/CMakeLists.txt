add_executable(_2_DirectAccess_publisher publisher.cpp)
add_executable(_2_DirectAccess_subscriber subscriber.cpp)
target_include_directories(_2_DirectAccess_publisher PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(_2_DirectAccess_subscriber PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(_2_DirectAccess_publisher PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_2_DirectAccess
    OUTPUT_NAME publisher
)
set_target_properties(_2_DirectAccess_subscriber PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_2_DirectAccess
    OUTPUT_NAME subscriber
)