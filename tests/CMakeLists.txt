# find gtest package
find_package(GTest CONFIG REQUIRED)

add_subdirectory(Core)
add_subdirectory(Platform)
add_subdirectory(Function)
add_subdirectory(CLI)
