set(TARGET_NAME JzREHeaderTool)

file(GLOB_RECURSE JzREHeaderTool_HEADERS "*.h")
file(GLOB_RECURSE JzREHeaderTool_SOURCES "*.cpp")

if(WIN32)
    set(LLVM_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/LLVM/lib/x64/libclang.lib)
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} /O2 /Ob2")
elseif(APPLE)
    set(LLVM_LIBS ${OSX_DEVELOPER_ROOT}/Toolchains/XcodeDefault.xctoolchain/usr/lib/libclang.dylib)
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O3")
elseif(LINUX)
    set(LLVM_LIBS ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/LLVM/bin/Linux/libclang.so.12)
    set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O3")
endif()

add_executable(
    ${TARGET_NAME}
    ${JzREHeaderTool_HEADERS}
    ${JzREHeaderTool_SOURCES}
)

target_compile_features(
    ${TARGET_NAME}
    PUBLIC
    cxx_std_20
)

target_include_directories(
    ${TARGET_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/LLVM/include 
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/mustache
    ${CMAKE_CURRENT_SOURCE_DIR}/source
)

target_link_libraries(
    ${TARGET_NAME}
    PUBLIC
    ${LLVM_LIBS}
)