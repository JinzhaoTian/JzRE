# EnttECS Module - EnTT-based Entity Component System
# This module provides an alternative ECS implementation using the EnTT library.

find_package(EnTT CONFIG REQUIRED)

file(GLOB JzREEnttECS_SOURCES CONFIGURE_DEPENDS 
    "src/*.cpp"
)

add_library(
    JzREEnttECS
    STATIC
    ${JzREEnttECS_SOURCES}
)

target_compile_features(
    JzREEnttECS
    PUBLIC
    cxx_std_20
)

target_include_directories(
    JzREEnttECS
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    JzREEnttECS
    PUBLIC
    JzRECore
    JzRERHI
    JzREResource
    EnTT::EnTT
)
