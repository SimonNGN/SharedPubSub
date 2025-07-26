cmake_minimum_required(VERSION 3.10)
project(SharedPubSub LANGUAGES CXX)

# Add the directory containing your header-only library
include_directories(${CMAKE_JS_INC})
include_directories(plugin)  # This line is crucial

# Include Node-API headers (cmake-js provides these automatically)
include_directories(${CMAKE_JS_INC})

# Include node-addon-api headers
execute_process(
    COMMAND node -p "require('node-addon-api').include"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE NODE_ADDON_API_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
include_directories(${NODE_ADDON_API_DIR})
include_directories(${CMAKE_SOURCE_DIR}/plugin)

# Define your Node.js addon target
add_library(SharedPubSub SHARED src/addon.cpp)

# Set the target name and properties for Node.js addon
set_target_properties(SharedPubSub PROPERTIES PREFIX "" SUFFIX ".node")

# Link against Node-API
target_link_libraries(SharedPubSub ${CMAKE_JS_LIB})

# Recommended: Disable C++ exceptions for N-API
target_compile_definitions(SharedPubSub PRIVATE NAPI_DISABLE_CPP_EXCEPTIONS)

# Set C++ standard
set_target_properties(SharedPubSub PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)