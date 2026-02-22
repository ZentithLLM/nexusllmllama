cmake_minimum_required(VERSION 3.22.1)
project(llama_android_bridge)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch Vulkan-Hpp (C++ bindings for Vulkan) for Android
include(FetchContent)
FetchContent_Declare(
    vulkan_hpp
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Hpp.git
    GIT_TAG        v1.3.275  # Match Vulkan header version in NDK
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(vulkan_hpp)

# Set up Vulkan variables for find_package in llama.cpp
set(Vulkan_INCLUDE_DIRS "${ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include")
set(Vulkan_LIBRARIES "vulkan")  # Link against -lvulkan (Android provides it)
set(Vulkan_FOUND TRUE)

# Add Vulkan-Hpp and Vulkan include directories globally
include_directories(${vulkan_hpp_SOURCE_DIR})
include_directories(${Vulkan_INCLUDE_DIRS})

add_subdirectory(llama.cpp)

add_library(llama_android_bridge SHARED
    jni_bridge.cpp
    bridge/generation_worker.cpp
)

target_include_directories(llama_android_bridge PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/include
    ${CMAKE_CURRENT_SOURCE_DIR}/llama.cpp/src
    ${CMAKE_CURRENT_SOURCE_DIR}/bridge
)

target_link_libraries(llama_android_bridge
    llama
    log
)