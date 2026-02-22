cmake_minimum_required(VERSION 3.22.1)
project(llama_android_bridge)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Vulkan for ggml
set(GGML_VULKAN ON CACHE BOOL "" FORCE)
set(GGML_VULKAN_USE_VOLK ON CACHE BOOL "" FORCE)

# Disable things not needed on Android
set(GGML_OPENMP OFF CACHE BOOL "" FORCE)
set(GGML_NATIVE OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

# Add llama.cpp submodule
add_subdirectory(llama.cpp)

# Your JNI bridge library
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
