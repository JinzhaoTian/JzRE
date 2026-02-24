# CMake Script Layout

This directory is organized by script role:

- `modules/`: reusable modules loaded through `include(...)`.
  - `JzSourceGroups.cmake`
  - `JzShaderCook.cmake`
- `precompile/`: header-tool precompile pipeline scripts.
  - `precompile.cmake`
  - `precompile.json.in`

Project entry points should append `cmake/modules` to `CMAKE_MODULE_PATH`.
