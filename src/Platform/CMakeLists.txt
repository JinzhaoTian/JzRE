# add platform-specific libraries
if(WIN32)
    set(PLATFORM_LIBS opengl32 gdi32)
elseif(APPLE)
    find_library(COCOA_LIB Cocoa)
    find_library(IOKIT_LIB IOKit)
    find_library(COREVIDEO_LIB CoreVideo)
    find_library(UNIFORMTYPEIDENTIFIERS_LIB UniformTypeIdentifiers)
    set(PLATFORM_LIBS ${COCOA_LIB} ${IOKIT_LIB} ${COREVIDEO_LIB} ${UNIFORMTYPEIDENTIFIERS_LIB})
elseif(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(X11 REQUIRED x11)
    set(PLATFORM_LIBS ${X11_LIBRARIES} ${CMAKE_DL_LIBS})
endif()

# set project platform-specific sources
if(WIN32)
    file(GLOB JzREPlatform_SOURCES CONFIGURE_DEPENDS 
         "Windows/*.cpp")
elseif(APPLE)
    file(GLOB JzREPlatform_SOURCES CONFIGURE_DEPENDS
         "macOS/*.cpp"
         "macOS/*.mm")
elseif(UNIX AND NOT APPLE)
    file(GLOB JzREPlatform_SOURCES CONFIGURE_DEPENDS
         "Linux/*.cpp")
endif()

add_library(JzREPlatform STATIC
    ${JzREPlatform_SOURCES}
)

target_include_directories(JzREPlatform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(JzREPlatform
    PRIVATE
    PLATFORM_LIBS
    JzRECore
)