# Runtime module (contains Platform, Core, Resource, Function layers)
add_subdirectory(Runtime)

# Optional CLI module
if(JzRE_BUILD_CLI)
    add_subdirectory(CLI)
endif()
