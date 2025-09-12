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

# set project source files
file(GLOB JzRE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Jz*.cpp")

# set project platform-specific sources
if(WIN32)
    file(GLOB PLATFORM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/platform/Windows/*.cpp")
elseif(APPLE)
    file(GLOB PLATFORM_SOURCES 
         "platform/macOS/*.cpp"
         "platform/macOS/*.mm")
elseif(UNIX AND NOT APPLE)
    file(GLOB PLATFORM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/platform/Linux/*.cpp")
endif()

# create library target
add_library(JzRELib STATIC ${JzRE_SOURCES} ${PLATFORM_SOURCES})

# set library target compile properties
target_compile_features(JzRELib PUBLIC cxx_std_20)

# set library target include path
target_include_directories(JzRELib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/"
)

# set library target link libraries
target_link_libraries(JzRELib PUBLIC
    ${PLATFORM_LIBS}
    ${OPENGL_LIBS}
    glad::glad
    glm::glm
    imgui::imgui
    assimp::assimp
    Threads::Threads
)

# set library target output
set_target_properties(JzRELib PROPERTIES
    OUTPUT_NAME "JzRE"
)

# create executable target
add_executable(JzRE "main.cpp")

# set executable target compile properties
target_compile_features(JzRE PUBLIC cxx_std_20)

# set executable target link library
target_link_libraries(JzRE PRIVATE JzRELib)

# set executable target output
set_target_properties(JzRE PROPERTIES
    OUTPUT_NAME "JzRE"
)