# set vcpkg triplet
if(NOT DEFINED VCPKG_TARGET_TRIPLET)
    if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
        set(VCPKG_TARGET_TRIPLET "x64-mingw-static")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(VCPKG_TARGET_TRIPLET "x64-linux")
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        if(CMAKE_OSX_ARCHITECTURES MATCHES "arm64")
            set(VCPKG_TARGET_TRIPLET "arm64-osx")
        else()
            set(VCPKG_TARGET_TRIPLET "x64-osx")
        endif()
    else()
        message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
    endif()
endif()

message(STATUS "Using vcpkg triplet: ${VCPKG_TARGET_TRIPLET}")

# set vcpkg toolchain
if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
else()
    find_program(VCPKG vcpkg)
    if(VCPKG)
        get_filename_component(VCPKG_ROOT ${VCPKG} DIRECTORY)
        set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "Vcpkg toolchain file")
    endif()
endif()

message(STATUS "Using vcpkg tool chain: ${CMAKE_TOOLCHAIN_FILE}")