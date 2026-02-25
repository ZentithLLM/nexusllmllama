# Custom FindVulkan module to handle Android NDK environment where standard module might fail
# or not find glslc correctly.
#
# This module assumes that the parent CMakeLists.txt (in app/src/main/cpp) has already:
# 1. Found glslc and set Vulkan_GLSLC_EXECUTABLE.
# 2. Handled linking logic for Android (libvulkan.so is standard).

if (ANDROID)
    message(STATUS "Custom FindVulkan: Android detected. Assuming Vulkan is available.")

    # Vulkan is considered found when building for Android with the NDK loader.
    set(Vulkan_FOUND TRUE)

    # Vulkan version:
    # - Default to the Vulkan 1.3 API, which matches the minimum API level required
    #   by this project.
    # - The exact header patch version (e.g. 1.3.296) is controlled by the
    #   Vulkan-Headers FetchContent() in the parent CMakeLists.txt.
    # - Allow overriding Vulkan_VERSION from the parent scope to avoid having to
    #   update this file when Vulkan-Headers is bumped.
    if (NOT DEFINED Vulkan_VERSION)
        set(Vulkan_VERSION "1.3")
    endif()
    # Define Vulkan::Vulkan target if not exists
    if (NOT TARGET Vulkan::Vulkan)
        add_library(Vulkan::Vulkan INTERFACE IMPORTED)
        set_target_properties(Vulkan::Vulkan PROPERTIES INTERFACE_LINK_LIBRARIES "vulkan")
    endif()

    # Define standard variables
    set(Vulkan_LIBRARY "vulkan")
    set(Vulkan_LIBRARIES "vulkan")

    # We don't set Vulkan_INCLUDE_DIRS because headers are either in sysroot
    # or provided via FetchContent (Vulkan-Headers) in parent scope.
    # llama.cpp uses Vulkan::Vulkan or includes <vulkan/vulkan.h>.

    # Check if glslc component was requested
    if (";${Vulkan_FIND_COMPONENTS};" MATCHES ";glslc;")
        if (Vulkan_GLSLC_EXECUTABLE)
             message(STATUS "Custom FindVulkan: Using provided glslc: ${Vulkan_GLSLC_EXECUTABLE}")
        else()
             if (Vulkan_FIND_REQUIRED_glslc OR Vulkan_FIND_REQUIRED)
                 message(WARNING "Custom FindVulkan: glslc requested but Vulkan_GLSLC_EXECUTABLE not set. Build might fail.")
             endif()
        endif()
    endif()

    return()
endif()

message(FATAL_ERROR "Custom FindVulkan.cmake is intended for Android build only.")
