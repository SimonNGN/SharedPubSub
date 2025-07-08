# my-header-only-addon/CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(MyHeaderOnlyAddon LANGUAGES CXX)

# Find Node.js and Node-API headers/libraries
find_package(Nodejs REQUIRED)

# Add the directory containing your header-only library
# This tells the compiler where to find "my_header_only_lib.h"
include_directories(plugin)

# Define your Node.js addon target
# 'myaddon' will be the name of your compiled .node file (myaddon.node)
# Add your addon's C++ source files here
Nodejs_ADDON_TARGET(SharedPubSub src/addon.cpp)

# Link against Node-API for Napi:: usage
target_link_libraries(SharedPubSub PRIVATE Nodejs::node_addon_api)

# Recommended: Disable C++ exceptions for N-API
# This helps maintain stability, as Node.js doesn't handle C++ exceptions directly
target_compile_definitions(SharedPubSub PRIVATE NAPI_DISABLE_CPP_EXCEPTIONS)

# Set C++ standard (optional, but good practice)
set_target_properties(SharedPubSub PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)