add_executable(_4_ObjectAtomic_publisher publisher.cpp)
add_executable(_4_ObjectAtomic_subscriber subscriber.cpp)
target_include_directories(_4_ObjectAtomic_publisher PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(_4_ObjectAtomic_subscriber PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(_4_ObjectAtomic_publisher PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_4_ObjectAtomic
    OUTPUT_NAME publisher
)
set_target_properties(_4_ObjectAtomic_subscriber PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_4_ObjectAtomic
    OUTPUT_NAME subscriber
)