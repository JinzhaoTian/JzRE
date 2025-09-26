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
    PUBLIC
    ${PLATFORM_LIBS}
    JzRECore
)