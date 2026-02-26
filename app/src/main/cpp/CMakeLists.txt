cmake_minimum_required(VERSION 3.22.1)
project(llama_android_bridge)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------
# Enable Vulkan backend
# ------------------------------------------------------------
set(GGML_VULKAN ON CACHE BOOL "" FORCE)

# ------------------------------------------------------------
# Fetch Vulkan C++ Headers (needed for vulkan.hpp)
# ------------------------------------------------------------
include(FetchContent)

FetchContent_Declare(
    vulkan_headers
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
    GIT_TAG v1.3.296
)

FetchContent_MakeAvailable(vulkan_headers)

# ------------------------------------------------------------
# Find glslc in Android NDK
# ------------------------------------------------------------
if (ANDROID AND DEFINED ANDROID_NDK)
    file(GLOB_RECURSE PROBABLE_GLSLC
        "${ANDROID_NDK}/shader-tools/*/glslc*"
    )
    if (PROBABLE_GLSLC)
        list(GET PROBABLE_GLSLC 0 GLSLC_FOUND)
        set(Vulkan_GLSLC_EXECUTABLE
            "${GLSLC_FOUND}"
            CACHE FILEPATH "glslc executable" FORCE
        )
        message(STATUS "Found NDK glslc: ${Vulkan_GLSLC_EXECUTABLE}")
    endif()
endif()

# ------------------------------------------------------------
# Add llama.cpp
# ------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake"
)

add_subdirectory(llama.cpp)

# ------------------------------------------------------------
# Fix ggml-vulkan include + link on Android
# ------------------------------------------------------------
if (ANDROID)
    if (TARGET ggml-vulkan)

        target_compile_definitions(ggml-vulkan PRIVATE
            VK_USE_PLATFORM_ANDROID_KHR
        )

        # Add Vulkan-Headers include path
        target_include_directories(ggml-vulkan PRIVATE
            ${vulkan_headers_SOURCE_DIR}/include
        )

        # Link Android system Vulkan
        target_link_libraries(ggml-vulkan PRIVATE
            vulkan
        )
    endif()
endif()

# ------------------------------------------------------------
# JNI bridge
# ------------------------------------------------------------
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
