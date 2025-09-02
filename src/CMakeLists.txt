# find packages using vcpkg
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(Threads REQUIRED)

# add platform-specific libraries and OpenGL
if(WIN32)
    find_package(OpenGL REQUIRED)
    set(PLATFORM_LIBS opengl32 gdi32)
    set(OPENGL_LIBS ${OpenGL})
elseif(APPLE)
    # On macOS, OpenGL is part of the system frameworks
    find_library(COCOA_LIB Cocoa)
    find_library(IOKIT_LIB IOKit)
    find_library(COREVIDEO_LIB CoreVideo)
    find_library(OPENGL_LIB OpenGL)
    find_library(GLUT_LIB GLUT)
    find_library(UNIFORMTYPEIDENTIFIERS_LIB UniformTypeIdentifiers)
    set(PLATFORM_LIBS ${COCOA_LIB} ${IOKIT_LIB} ${COREVIDEO_LIB} ${UNIFORMTYPEIDENTIFIERS_LIB})
    set(OPENGL_LIBS ${OPENGL_LIB} ${GLUT_LIB})
elseif(UNIX AND NOT APPLE)
    find_package(OpenGL REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(X11 REQUIRED x11)
    set(PLATFORM_LIBS ${X11_LIBRARIES} ${CMAKE_DL_LIBS})
    set(OPENGL_LIBS ${OpenGL})
endif()

# set project include path
include_directories(${PROJECT_SOURCE_DIR}/src)

# set project source files
file(GLOB JZRE_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

# set project platform-specific sources
if(WIN32)
    file(GLOB PLATFORM_SOURCES "${PROJECT_SOURCE_DIR}/src/platform/Windows/*.cpp")
elseif(APPLE)
    file(GLOB PLATFORM_SOURCES 
         "${PROJECT_SOURCE_DIR}/src/platform/macOS/*.cpp"
         "${PROJECT_SOURCE_DIR}/src/platform/macOS/*.mm")
elseif(UNIX AND NOT APPLE)
    file(GLOB PLATFORM_SOURCES "${PROJECT_SOURCE_DIR}/src/platform/Linux/*.cpp")
endif()

# include generated reflection files
if(REFLECTION_OUTPUT_DIRECTORY)
    include_directories(${REFLECTION_OUTPUT_DIRECTORY})
    message(STATUS "添加反射代码包含目录: ${REFLECTION_OUTPUT_DIRECTORY}")
endif()

# combine all sources including generated reflection files
set(ALL_SOURCES ${JZRE_SOURCES} ${PLATFORM_SOURCES})
if(REFLECTION_GENERATED_FILES)
    list(APPEND ALL_SOURCES ${REFLECTION_GENERATED_FILES})
    list(LENGTH REFLECTION_GENERATED_FILES GENERATED_COUNT)
    message(STATUS "包含 ${GENERATED_COUNT} 个生成的反射文件")
endif()

# add exe target
add_executable(JzRE "${PROJECT_SOURCE_DIR}/src/main.cpp" ${ALL_SOURCES})

# 添加反射代码生成依赖
if(REFLECTION_TARGET)
    add_dependencies(JzRE ${REFLECTION_TARGET})
    message(STATUS "添加反射代码生成依赖: ${REFLECTION_TARGET}")
endif()

# 设置目标属性，确保反射头文件可被找到
target_include_directories(JzRE PRIVATE 
    ${PROJECT_SOURCE_DIR}/src
    $<$<BOOL:${REFLECTION_OUTPUT_DIRECTORY}>:${REFLECTION_OUTPUT_DIRECTORY}>
)

# set target properties
set_target_properties(JzRE PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# link libraries using vcpkg targets
target_link_libraries(JzRE PRIVATE
    ${PLATFORM_LIBS}
    ${OPENGL_LIBS}
    glad::glad
    glm::glm
    imgui::imgui
    assimp::assimp
    Threads::Threads
)

# set output directory
set_target_properties(JzRE PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)

# copy resources
add_custom_command(
    TARGET JzRE POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:JzRE>/assets
    COMMENT "copying resources to output directory"
)