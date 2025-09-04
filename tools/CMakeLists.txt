find_package(LLVM CONFIG REQUIRED)
find_package(Clang CONFIG REQUIRED)

message(STATUS "Found LLVM ${LLVM_FOUND}")
message(STATUS "LLVM version: ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM libraries dirs: ${LLVM_LIBRARY_DIRS}")
message(STATUS "LLVM libraries: ${LLVM_LIBRARY}")

message(STATUS "Found Clang ${Clang_FOUND}")
message(STATUS "Clang version: ${Clang_VERSION}")
message(STATUS "Clang include dirs: ${CLANG_INCLUDE_DIRS}")
message(STATUS "Clang library dirs: ${CLANG_LIBRARY_DIRS}")
message(STATUS "Clang libraries: ${CLANG_LIBRARIES}")

include_directories(
    ${LLVM_INCLUDE_DIRS}
    ${CLANG_INCLUDE_DIRS}
)

llvm_map_components_to_libnames(LLVM_LIBS
    support
    core 
    irreader 
    option
)

set(CLANG_LIBS
    clangFrontendTool
    clangFrontend 
    clangDriver 
    clangSerialization
    clangParse 
    clangSema 
    clangAnalysis 
    clangAST 
    clangASTMatchers
    clangRewrite 
    clangLex 
    clangBasic 
    clangEdit
    clangTooling
) 

add_executable(JzREHeaderTool
    JzREHeaderTool.cpp
)

target_link_libraries(JzREHeaderTool PRIVATE
    ${LLVM_LIBS}
    ${CLANG_LIBS}
)

# set output directory
set_target_properties(JzREHeaderTool PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)
