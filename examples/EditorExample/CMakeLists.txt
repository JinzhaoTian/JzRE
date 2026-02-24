cmake_minimum_required(VERSION 3.25.0)

project(EditorExample VERSION 0.0.1 LANGUAGES CXX C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
    add_link_options("-Wl,-no_warn_duplicate_libraries")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(JzRE_REPOSITORY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set(JzRE_OUTPUT_DIR ${CMAKE_BINARY_DIR}/EditorExample)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${JzRE_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${JzRE_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${JzRE_OUTPUT_DIR})

foreach(CONFIG_TYPE DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${JzRE_OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${JzRE_OUTPUT_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_TYPE} ${JzRE_OUTPUT_DIR})
endforeach()

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resources/" DESTINATION ${JzRE_OUTPUT_DIR}/)

add_subdirectory("${JzRE_REPOSITORY_ROOT}/src/Runtime" "${CMAKE_CURRENT_BINARY_DIR}/Runtime")

add_subdirectory(Core)
add_subdirectory(UI)
add_subdirectory(Panels)
add_subdirectory(Application)

add_executable(
    EditorExample
    main.cpp
)

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES main.cpp
)

set_target_properties(EditorExample PROPERTIES FOLDER "Examples/EditorExample")

target_compile_features(
    EditorExample
    PUBLIC
    cxx_std_20
)

target_link_libraries(
    EditorExample
    PUBLIC
    JzEditor
)
