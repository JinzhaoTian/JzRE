find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVM libraries: ${LLVM_LIBRARY_DIRS}")
message(STATUS "Using Clang libraries: ${CLANG_LIBRARY_DIRS}")

include_directories(
    ${LLVM_INCLUDE_DIRS}
    ${CLANG_INCLUDE_DIRS}
)

llvm_map_components_to_libnames(llvm_libs
    support core irreader option
)

llvm_map_components_to_libnames(clang_libs
    clangFrontendTool clangFrontend clangDriver clangSerialization
    clangParse clangSema clangAnalysis clangAST clangASTMatchers
    clangRewrite clangLex clangBasic clangEdit clangTooling
) 

add_executable(JzREHeaderTool
    JzREHeaderTool.cpp
)

target_link_libraries(JzREHeaderTool
    PRIVATE
    ${llvm_libs}
    ${clang_libs}
)

# set output directory
set_target_properties(JzREHeaderTool PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)

# 安装目标（可选）
# install(TARGETS ReflectionGenerator DESTINATION bin)