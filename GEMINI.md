# Gemini Project Analysis: JzRE

## 1. Project Overview

-   **Project Name**: JzRE
-   **Description**: A real-time rendering engine and editor built in C++. The name suggests JzRE.
-   **Core Language**: C++ (C++20 standard)
-   **Documentation**: Mandatory, English, Doxygen format

## 2. Build & Dependency Management

-   **Build System**: CMake
-   **Package Manager**: vcpkg
-   **Build Output**: Executables and libraries are placed in the `build/JzRE` directory.
-   **Build Presets**: The project uses `CMakePresets.json` for configuring builds across different platforms (macOS, Linux, Windows). For macOS, the recommend presets are `macos-clang-static` and `macos-clang-dynamic`. For Windows, the relevant presets are `windows-msvc-static` and `windows-msvc-dynamic`.

### Dependencies (`vcpkg.json`)

-   **Windowing & Input**: `glfw3`
-   **Graphics API Loader**:
    -   `glad` (for OpenGL)
-   **Math**: `JzVector` and `JzMatrix`
-   **UI**: `imgui` (with docking and OpenGL/GLFW bindings)
-   **Model Loading**: `assimp`
-   **Image/Texture Loading**: `stb`
-   **Logging**: `JzLogger`
-   **Serialization:** `nlohmann-json`
-   **Testing:** `gtest`

## 3. Project Structure

-   `src/`: Contains the core source code for the engine and editor. The code is organized into a modular structure with classes prefixed by `Jz`, covering areas like Rendering (`JzRHI*`, `JzOpenGL*`), UI (`JzButton`, `JzPanel*`), and application logic (`JzEditor`, `JzREHub`).
-   `tests/`: Contains tests for the project (currently empty or not listed).
-   `examples/`: Contains example usage of the engine's features.
-   `docs/`: Contains project documentation (architecture, design, etc.).
-   `resources/`: Contains assets used by the engine, such as shaders, models, and textures. This directory is copied to the build output directory.
-   `build/`: The default directory for all build artifacts.

## 4. Build and Run Commands

Based on your `CMakePresets.json` and `CMakeLists.txt`:

**1. Configure the build (run once):**

-   For macOS, you can use the `macos-clang-static` preset.

```bash
cmake --preset macos-clang-static
```

-   For Windows, you can use the `windows-msvc-static` preset.

```bash
cmake --preset windows-msvc-static
```

**2. Build the project:**

-   This command should be run from the project root.

```bash
cmake --build build
```

**3. Run the application:**

-   The main executable will be located in the build output directory.

```bash
./build/JzRE/JzRE
```

## 5. Code Standards

-   **Code Style**: The presence of `.clang-format` indicates that the project uses ClangFormat to maintain a consistent code style.
-   **Readability**: Prioritize code readability and maintainability. Use meaningful variable and function names.
-   **Modularity**: Decompose functionality into small, focused functions or classes, following the single responsibility principle.
-   **Error Handling**: Must include appropriate error handling mechanisms (e.g., exceptions, error code returns) and consider edge cases.
-   **Memory Safety**: If using manual memory management, ensure no leaks; prioritize modern C++ features like smart pointers.

## 6. Comments and Documentation

-   **Doxygen Specification**: All comments for file headers, classes, functions, enums, etc., **must** strictly conform to the Doxygen specification.
-   **Comment Language**: All comments (including file headers and inline comments) **must** be written in **English**.
-   **Comment Content**:
    -   File Header Comment: Must include `@author`, `@copyright`.
    -   Function Comment: Must include `@brief`, `@param` (for each parameter), `@return` (return value), and `@throw` (exceptions thrown).
    -   Key Algorithms or Complex Logic: Add clear English inline comments within the code block to explain the "why" behind the logic, not just the "what".

## 7. Development Principles

### a. Compatibility First

-   **Cross-Platform:** The engine must be compatible with multiple operating systems, including Windows, macOS, and Linux.
-   **Multi-API Support:** The rendering backend should support multiple graphics APIs (e.g., OpenGL, Vulkan, Metal, DirectX). At present, it can only support OpenGL.

### b. Rendering Architecture: RHI

-   To achieve multi-API support, all rendering operations must be abstracted through a **Rendering Hardware Interface (RHI)** layer.
-   This abstraction layer isolates engine logic from the specific details of the underlying graphics API.
-   All classes that are part of this abstraction layer must be prefixed with `JzRHI*` (e.g., `JzRHIDevice`, `JzRHIPipeline`).

### c. UI Architecture: Stateful Abstraction

-   The immediate-mode `imgui` library should be wrapped to create a stateful, object-oriented UI framework.
-   This involves creating classes for UI elements (e.g., `JzButton`, `JzPanel`) that manage their own state and behavior, providing a more declarative and structured way to build user interfaces.

## 8. Output Format Requirements

-   **Code Structure**: Provide complete, compilable code snippets or files. If it's just a function, provide its complete definition.
-   **Markup Language**: In your response, enclose the generated code using **`cpp ... `** code fence format.
