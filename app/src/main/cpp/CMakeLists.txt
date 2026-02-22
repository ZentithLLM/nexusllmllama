cmake_minimum_required(VERSION 3.22.1)
project(llama_android_bridge)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Android Vulkan Fix ----
# Force dynamic Vulkan loading via Volk
set(GGML_VULKAN ON CACHE BOOL "" FORCE)
set(GGML_VULKAN_USE_VOLK ON CACHE BOOL "" FORCE)
set(VK_NO_PROTOTYPES ON CACHE BOOL "" FORCE)

# Prevent static Vulkan linking on Android
set(Vulkan_FOUND FALSE)

# ---- Add llama.cpp submodule ----
add_subdirectory(llama.cpp)

# ---- Fix ggml-vulkan linking on Android ----
if (ANDROID)
    if (TARGET ggml-vulkan)
        target_compile_definitions(ggml-vulkan PRIVATE
            VK_NO_PROTOTYPES
            VK_USE_PLATFORM_ANDROID_KHR
        )

        # Remove static Vulkan link
        get_target_property(LIBS ggml-vulkan LINK_LIBRARIES)
        if (LIBS)
            list(REMOVE_ITEM LIBS Vulkan::Vulkan)
            set_target_properties(ggml-vulkan PROPERTIES LINK_LIBRARIES "${LIBS}")
        endif()
    endif()
endif()

# ---- Your JNI bridge ----
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
