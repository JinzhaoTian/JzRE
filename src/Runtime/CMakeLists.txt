# Runtime module - combines all runtime layers
add_subdirectory(Core)
add_subdirectory(Platform)
add_subdirectory(Resource)
add_subdirectory(Function)

# Create an interface library that links all runtime layers
add_library(JzRuntime INTERFACE)
target_link_libraries(JzRuntime INTERFACE
    JzRuntimeCore
    JzRuntimePlatform
    JzRuntimeResource
    JzRuntimeFunction
)
