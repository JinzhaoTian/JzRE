# 设置自定义triplet目录
set(CUSTOM_TRIPLETS_DIR "${CMAKE_SOURCE_DIR}/cmake")
set(VCPKG_OVERLAY_TRIPLETS "${CUSTOM_TRIPLETS_DIR}" CACHE STRING "")

# 根据平台自动选择triplet
if(NOT DEFINED VCPKG_TARGET_TRIPLET)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(VCPKG_TARGET_TRIPLET "x64-windows-dynamic")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(VCPKG_TARGET_TRIPLET "x64-linux-dynamic")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        if(CMAKE_OSX_ARCHITECTURES MATCHES "arm64")
            set(VCPKG_TARGET_TRIPLET "arm64-osx-dynamic")
        else()
            set(VCPKG_TARGET_TRIPLET "x64-osx") # 默认x64
        endif()
    else()
        message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
    endif()
endif()

# 打印当前使用的triplet
message(STATUS "Using vcpkg triplet: ${VCPKG_TARGET_TRIPLET}")

# 设置vcpkg工具链
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
elseif(DEFINED ENV{VCPKG_INSTALLATION_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
else()
    # 尝试自动查找vcpkg
    find_program(VCPKG vcpkg)
    if(VCPKG)
        get_filename_component(VCPKG_ROOT ${VCPKG} DIRECTORY)
        set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
    endif()
endif()

# 确保自定义triplet目录存在
if(NOT EXISTS "${CUSTOM_TRIPLETS_DIR}")
    message(WARNING "Custom triplets directory not found: ${CUSTOM_TRIPLETS_DIR}")
endif()