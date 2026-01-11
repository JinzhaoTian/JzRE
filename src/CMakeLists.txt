# Runtime module (contains Platform, Core, Resource, Function layers)
add_subdirectory(Runtime)

# Editor module (depends on Runtime)
add_subdirectory(Editor)

# Application entry points
add_subdirectory(App)