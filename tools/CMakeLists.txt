find_package(LLVM CONFIG REQUIRED)
find_package(Clang CONFIG REQUIRED)

message(STATUS "LLVM version: ${LLVM_PACKAGE_VERSION}")
message(STATUS "Clang version: ${Clang_VERSION}")

add_executable(JzREHeaderTool
    JzREHeaderTool.cpp
)

target_include_directories(JzREHeaderTool PRIVATE
    ${LLVM_INCLUDE_DIRS}
    ${CLANG_INCLUDE_DIRS}
)

target_compile_definitions(JzREHeaderTool PRIVATE
    ${LLVM_DEFINITIONS}
    ${CLANG_DEFINITIONS}
)

# Disable RTTI to match LLVM configuration
if(MSVC)
    # MSVC
    target_compile_options(JzREHeaderTool PRIVATE
        /GR
        /EHsc
    )
else()
    # GCC/Clang
    target_compile_options(JzREHeaderTool PRIVATE
        -fno-rtti
        -fexceptions
    )
endif()

target_link_libraries(JzREHeaderTool PRIVATE
    clangTooling
    clangFrontend
    clangDriver
)

 # set output directory
set_target_properties(JzREHeaderTool PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
