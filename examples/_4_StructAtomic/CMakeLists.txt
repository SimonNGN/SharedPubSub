add_executable(_4_StructAtomic_publisher publisher.cpp)
add_executable(_4_StructAtomic_subscriber subscriber.cpp)
target_include_directories(_4_StructAtomic_publisher PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(_4_StructAtomic_subscriber PRIVATE ${CMAKE_SOURCE_DIR})
set_target_properties(_4_StructAtomic_publisher PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_4_StructAtomic
    OUTPUT_NAME publisher
)
set_target_properties(_4_StructAtomic_subscriber PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_4_StructAtomic
    OUTPUT_NAME subscriber
)