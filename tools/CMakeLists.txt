cmake_minimum_required(VERSION 3.16)
project(JzREHeaderTool)

# Set C++ standard to C++20 for LLVM compatibility
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Try to find LLVM through vcpkg first
find_package(LLVM CONFIG QUIET)
find_package(Clang CONFIG QUIET)

# Force system LLVM for now due to vcpkg RTTI issues on ARM64 macOS
if(FALSE AND LLVM_FOUND AND Clang_FOUND)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION} via vcpkg")
    message(STATUS "Using LLVM libraries: ${LLVM_LIBRARY_DIRS}")
    message(STATUS "Using Clang libraries: ${CLANG_LIBRARY_DIRS}")

    include_directories(
        ${LLVM_INCLUDE_DIRS}
        ${CLANG_INCLUDE_DIRS}
    )
    
    # Ensure LLVM definitions are properly set for vcpkg
    add_definitions(${LLVM_DEFINITIONS})

    llvm_map_components_to_libnames(llvm_libs
        support core irreader option
        mcparser mc object objectyaml bitreader bitwriter
        transformutils frontendopenmp
    )

    # Clang libraries - use direct library names with all dependencies
    set(clang_libs
        clangFrontend clangDriver clangSerialization
        clangParse clangSema clangAnalysis clangAST clangASTMatchers
        clangRewrite clangLex clangBasic clangEdit clangTooling
        clangAPINotes clangSupport clangToolingCore
        clangFrontendTool clangCodeGen clangStaticAnalyzerFrontend
        clangStaticAnalyzerCheckers clangStaticAnalyzerCore
    )
    
    # Add system libraries required by LLVM/Clang
    if(APPLE)
        find_library(ZLIB_LIB z)
        if(ZLIB_LIB)
            list(APPEND llvm_libs ${ZLIB_LIB})
        endif()
    endif()
    
    set(LLVM_LINKING_METHOD "vcpkg")

else()
    # Fallback to system LLVM or manual configuration
    message(STATUS "LLVM not found via vcpkg, trying system installation...")
    
    # Look for LLVM installation in common locations
    set(LLVM_ROOT_HINTS
        $ENV{LLVM_ROOT}
        $ENV{LLVM_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/llvm
        ${CMAKE_CURRENT_SOURCE_DIR}/../deps/llvm
        "C:/Program Files/LLVM"
        "C:/tools/LLVM"
        "/usr/lib/llvm-17"
        "/usr/lib/llvm-16"
        "/usr/lib/llvm-15"
        "/opt/homebrew/opt/llvm"
        "/usr/local/opt/llvm"
    )

    find_path(LLVM_INCLUDE_DIR
        NAMES llvm/Support/CommandLine.h
        HINTS ${LLVM_ROOT_HINTS}
        PATH_SUFFIXES include
        DOC "LLVM include directory"
    )

    find_path(CLANG_INCLUDE_DIR
        NAMES clang/AST/ASTConsumer.h
        HINTS ${LLVM_ROOT_HINTS}
        PATH_SUFFIXES include
        DOC "Clang include directory"
    )

    # Find LLVM libraries
    find_library(LLVM_SUPPORT_LIB
        NAMES LLVMSupport libLLVMSupport
        HINTS ${LLVM_ROOT_HINTS}
        PATH_SUFFIXES lib lib64
    )

    # Find essential Clang libraries
    find_library(CLANG_TOOLING_LIB
        NAMES clangTooling libclangTooling
        HINTS ${LLVM_ROOT_HINTS}
        PATH_SUFFIXES lib lib64
    )

    if(LLVM_INCLUDE_DIR AND CLANG_INCLUDE_DIR AND LLVM_SUPPORT_LIB AND CLANG_TOOLING_LIB)
        message(STATUS "Found LLVM system installation")
        message(STATUS "LLVM include: ${LLVM_INCLUDE_DIR}")
        message(STATUS "Clang include: ${CLANG_INCLUDE_DIR}")

        include_directories(${LLVM_INCLUDE_DIR} ${CLANG_INCLUDE_DIR})

        # Manual library linking for common LLVM libraries
        # Try to use llvm-config if available
        find_program(LLVM_CONFIG_EXECUTABLE
            NAMES llvm-config llvm-config-20 llvm-config-19 llvm-config-18
            HINTS ${LLVM_ROOT_HINTS}
            PATH_SUFFIXES bin
        )
        
        if(LLVM_CONFIG_EXECUTABLE)
            execute_process(
                COMMAND ${LLVM_CONFIG_EXECUTABLE} --libdir
                OUTPUT_VARIABLE LLVM_LIBDIR_OUTPUT
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            execute_process(
                COMMAND ${LLVM_CONFIG_EXECUTABLE} --system-libs
                OUTPUT_VARIABLE LLVM_SYSTEM_LIBS_OUTPUT
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            
            # Use the dynamic library instead of static components
            find_library(LLVM_MAIN_LIB
                NAMES LLVM libLLVM
                PATHS ${LLVM_LIBDIR_OUTPUT}
                NO_DEFAULT_PATH
            )
            
            if(LLVM_MAIN_LIB)
                set(llvm_libs ${LLVM_MAIN_LIB})
                # Add system libs if needed
                if(LLVM_SYSTEM_LIBS_OUTPUT)
                    string(REPLACE " " ";" SYSTEM_LIBS_LIST "${LLVM_SYSTEM_LIBS_OUTPUT}")
                    list(APPEND llvm_libs ${SYSTEM_LIBS_LIST})
                endif()
                message(STATUS "Using LLVM dynamic library: ${LLVM_MAIN_LIB}")
            else()
                set(llvm_libs ${LLVM_SUPPORT_LIB})
            endif()
        else()
            set(llvm_libs ${LLVM_SUPPORT_LIB})
        endif()

        # Find Clang libraries manually
        set(clang_lib_names
            clangFrontend clangDriver clangSerialization
            clangParse clangSema clangAnalysis clangAST clangASTMatchers
            clangRewrite clangLex clangBasic clangEdit clangTooling
            clangAPINotes clangSupport clangToolingCore
            clangFrontendTool clangCodeGen clangStaticAnalyzerFrontend
            clangStaticAnalyzerCheckers clangStaticAnalyzerCore
        )

        set(clang_libs "")
        foreach(lib_name ${clang_lib_names})
            find_library(${lib_name}_LIB
                NAMES ${lib_name} lib${lib_name}
                HINTS ${LLVM_ROOT_HINTS}
                PATH_SUFFIXES lib lib64
            )
            if(${lib_name}_LIB)
                list(APPEND clang_libs ${${lib_name}_LIB})
            endif()
        endforeach()

        set(LLVM_LINKING_METHOD "system")
    else()
        message(FATAL_ERROR "LLVM not found! Please install LLVM or set LLVM_ROOT environment variable")
    endif()
endif() 

add_executable(JzREHeaderTool
    JzREHeaderTool.cpp
)

# Ensure RTTI is enabled for typeinfo symbols and vcpkg compatibility
target_compile_options(JzREHeaderTool PRIVATE
    $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-frtti>
    $<$<CXX_COMPILER_ID:MSVC>:/GR>
)

# For vcpkg LLVM, we need to ensure proper symbol visibility
if(LLVM_LINKING_METHOD STREQUAL "vcpkg")
    target_compile_definitions(JzREHeaderTool PRIVATE
        ${LLVM_DEFINITIONS}
        -D__STDC_CONSTANT_MACROS
        -D__STDC_FORMAT_MACROS  
        -D__STDC_LIMIT_MACROS
    )
endif()

target_link_libraries(JzREHeaderTool
    PRIVATE
    ${clang_libs}
    ${llvm_libs}
)

# Platform specific linking options
if(APPLE)
    target_link_options(JzREHeaderTool PRIVATE 
        -Wl,-no_warn_duplicate_libraries
        -Wl,-search_paths_first
        -Wl,-headerpad_max_install_names
    )
    # Add required system frameworks and libraries for LLVM/Clang on macOS
    target_link_libraries(JzREHeaderTool PRIVATE
        "-framework CoreFoundation"
        curses
        z
        m
    )
endif()

# set output directory
set_target_properties(JzREHeaderTool PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)

# 安装目标（可选）
# install(TARGETS ReflectionGenerator DESTINATION bin)