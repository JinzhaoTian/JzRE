# Runtime module - combines all runtime layers
add_subdirectory(Core)
add_subdirectory(Platform)
add_subdirectory(Resource)
add_subdirectory(Function)
add_subdirectory(Interface)

# Create an interface library that links all runtime layers
add_library(JzRERuntime INTERFACE)

target_link_libraries(JzRERuntime 
    INTERFACE
    JzRuntimeCore
    JzRuntimePlatform
    JzRuntimeResource
    JzRuntimeFunction
    JzREInterface
)
