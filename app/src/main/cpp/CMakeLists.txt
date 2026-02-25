cmake_minimum_required(VERSION 3.22.1)
project(llama_android_bridge)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Add Vulkan Headers for Android ----
include(FetchContent)
FetchContent_Declare(
    Vulkan-Headers
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
    GIT_TAG        v1.3.296
)
FetchContent_MakeAvailable(Vulkan-Headers)

# ---- Find glslc in NDK ----
if (ANDROID AND DEFINED ANDROID_NDK)
    file(GLOB_RECURSE PROBABLE_GLSLC "${ANDROID_NDK}/shader-tools/*/glslc*")
    if (PROBABLE_GLSLC)
        list(GET PROBABLE_GLSLC 0 GLSLC_FOUND)
        set(Vulkan_GLSLC_EXECUTABLE "${GLSLC_FOUND}" CACHE FILEPATH "glslc executable" FORCE)
        message(STATUS "Found NDK glslc: ${Vulkan_GLSLC_EXECUTABLE}")
    endif()
endif()

# ---- Android Vulkan Fix ----
# Force dynamic Vulkan loading via Volk
set(GGML_VULKAN ON CACHE BOOL "" FORCE)
set(GGML_VULKAN_USE_VOLK ON CACHE BOOL "" FORCE)
set(VK_NO_PROTOTYPES ON CACHE BOOL "" FORCE)

# Prevent static Vulkan linking on Android
# set(Vulkan_FOUND FALSE) # Disabled to allow finding Vulkan

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
        # get_target_property(LIBS ggml-vulkan LINK_LIBRARIES)
        # if (LIBS)
        #    list(REMOVE_ITEM LIBS Vulkan::Vulkan)
        #    set_target_properties(ggml-vulkan PROPERTIES LINK_LIBRARIES "${LIBS}")
        # endif()

        # Link Vulkan-Headers to provide missing headers
        if (TARGET Vulkan::Headers)
             target_link_libraries(ggml-vulkan PRIVATE Vulkan::Headers)
        endif()

        # Link against vulkan just in case, though VK_NO_PROTOTYPES handles most symbols via dispatcher
        target_link_libraries(ggml-vulkan PRIVATE vulkan)
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
