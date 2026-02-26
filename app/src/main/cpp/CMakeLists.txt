cmake_minimum_required(VERSION 3.22.1)
project(llama_android_bridge)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------
# Enable Vulkan backend (Android uses system libvulkan.so)
# ------------------------------------------------------------
set(GGML_VULKAN ON CACHE BOOL "" FORCE)

# IMPORTANT:
# Do NOT enable Volk on Android
# Do NOT define VK_NO_PROTOTYPES
# Android NDK provides proper Vulkan loader

# ------------------------------------------------------------
# Find glslc inside Android NDK (for shader compilation)
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
# Android-specific Vulkan linking
# ------------------------------------------------------------
if (ANDROID)
    if (TARGET ggml-vulkan)

        # Required for Android surface support
        target_compile_definitions(ggml-vulkan PRIVATE
            VK_USE_PLATFORM_ANDROID_KHR
        )

        # Link against Android system Vulkan
        target_link_libraries(ggml-vulkan PRIVATE
            vulkan
        )

    endif()
endif()

# ------------------------------------------------------------
# JNI Bridge
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
