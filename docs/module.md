## 1. 建议的目录结构

```
JzRE/
├── src/
│   ├── Core/                # 核心工具与类型 (日志, 事件, 矩阵/向量, 核心类型)
│   │   ├── CMakeLists.txt
│   │   └── ... (JzLogger.h/cpp, JzEvent.h, JzMatrix.h, JzRETypes.h, ...)
│   ├── RHI/                 # 渲染硬件接口 (抽象层)
│   │   ├── CMakeLists.txt
│   │   └── ... (JzRHIDevice.h, JzRHIPipeline.h, JzRHIBuffer.h, ...)
│   ├── Render/
│   │   └── OpenGL/          # OpenGL具体实现
│   │       ├── CMakeLists.txt
│   │       └── ... (JzOpenGLDevice.h/cpp, JzOpenGLBuffer.h/cpp, ...)
│   ├── Scene/               # 场景与组件 (实体, 组件, 模型, 材质, 相机)
│   │   ├── CMakeLists.txt
│   │   └── ... (JzEntity.h, JzComponent.h, JzModel.h/cpp, JzMesh.h/cpp, ...)
│   ├── Resource/            # 资源管理
│   │   ├── CMakeLists.txt
│   │   └── ... (JzResourceManager.h/cpp, JzResource.h, JzFactories, ...)
│   ├── UI/                  # UI组件封装
│   │   ├── CMakeLists.txt
│   │   └── ... (JzPanel.h/cpp, JzButton.h/cpp, JzWidget.h, ...)
│   ├── Editor/              # 编辑器逻辑
│   │   ├── CMakeLists.txt
│   │   └── ... (JzEditor.h/cpp, JzHierarchy.h/cpp, JzAssetBrowser.h/cpp, ...)
│   ├── App/                 # 主程序入口
│   │   ├── CMakeLists.txt
│   │   └── JzREHub.cpp      # 主程序 main 函数所在
│   └── CMakeLists.txt       # src 的主 CMakeLists, 用于串联所有子模块
│
├── tests/
│   ├── Core/                # 针对 Core 模块的测试
│   │   ├── CMakeLists.txt
│   │   └── test_logger.cpp
│   ├── Scene/               # 针对 Scene 模块的测试
│   │   ├── CMakeLists.txt
│   │   └── test_entity.cpp
│   └── CMakeLists.txt       # tests 的主 CMakeLists
│
└── CMakeLists.txt           # 项目根 CMakeLists
```

## 2. CMake 实现方案

### 根 `CMakeLists.txt`

这个文件保持不变，但要确保它包含了 src 和 tests 子目录。

```cmake

# D:/repos/JzRE/CMakeLists.txt

cmake_minimum_required(VERSION 3.20)
project(JzRE LANGUAGES CXX)

# ... vcpkg and other global settings ...

add_subdirectory(src)

# Enable testing and add the tests directory

enable_testing()
add_subdirectory(tests)
```

### `src` 的主 `CMakeLists.txt`

这个文件变成一个“调度中心”，按依赖顺序添加所有子模块。

```cmake

# D:/repos/JzRE/src/CMakeLists.txt

add_subdirectory(Core)
add_subdirectory(RHI)
add_subdirectory(Render/OpenGL)
add_subdirectory(Scene)
add_subdirectory(Resource)
add_subdirectory(UI)
add_subdirectory(Editor)
add_subdirectory(App)
```

### 模块 `CMakeLists.txt` 示例

a. `Core` 模块 (基础库, 无内部依赖)

```cmake

# D:/repos/JzRE/src/Core/CMakeLists.txt

add_library(JzCore STATIC)

target_sources(JzCore PRIVATE
    JzClock.cpp
    JzLogger.cpp # ... 其他核心工具的 .cpp 文件
)

target_include_directories(JzCore
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

# JzMatrix 和 JzVector 是头文件库，如果它们依赖 vcpkg 的库，可以在这里链接

target_link_libraries(JzCore PUBLIC
    JzMatrix::JzMatrix # 假设 JzMatrix 是一个 interface
    library
    JzVector::JzVector
    JzLogger::JzLogger # 假设 JzLogger 依赖 spdlog
 )
```

b. `RHI` 模块 (纯接口库)

由于 RHI 是纯抽象层，只有头文件，最适合定义为 INTERFACE 库。

```cmake

# D:/repos/JzRE/src/RHI/CMakeLists.txt

add_library(JzRHI INTERFACE)

target_include_directories(JzRHI INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# RHI 依赖核心类型

target_link_libraries(JzRHI INTERFACE
    JzCore
)
```

c. `OpenGL` 模块 (依赖 `RHI` 和 `Core`)

```cmake

# D:/repos/JzRE/src/Render/OpenGL/CMakeLists.txt

add_library(JzRenderOpenGL STATIC)

target_sources(JzRenderOpenGL PRIVATE
    JzOpenGLDevice.cpp
    JzOpenGLBuffer.cpp
    JzOpenGLPipeline.cpp # ... 其他 OpenGL 实现的 .cpp
)

target_include_directories(JzRenderOpenGL PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接抽象层 RHI, 核心库 Core, 以及外部库 glad 和 glfw

target_link_libraries(JzRenderOpenGL
    PUBLIC
    JzRHI
    PRIVATE
    JzCore
    glad::glad
    glfw
)
```

d. `Editor` 模块 (依赖多个内部模块)

```cmake

# D:/repos/JzRE/src/Editor/CMakeLists.txt

add_library(JzEditor STATIC)

target_sources(JzEditor PRIVATE
    JzEditor.cpp
    JzHierarchy.cpp
    JzAssetBrowser.cpp # ...
)

target_include_directories(JzEditor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 编辑器模块依赖渲染、场景、资源和 UI

target_link_libraries(JzEditor
    PUBLIC
    JzRenderOpenGL # 或者只暴露 JzRHI，具体实现设为 PRIVATE
    JzScene
    JzResource
    JzUI
    PRIVATE
    JzCore
)
```

e. `App` 模块 (最终可执行文件)

```cmake

# D:/repos/JzRE/src/App/CMakeLists.txt

add_executable(JzRE
    JzREHub.cpp
)

# 链接编辑器库，它会传递所有其他需要的依赖

target_link_libraries(JzRE
    PRIVATE
    JzEditor
)
```

### `tests` 的 `CMakeLists.txt` 示例

```cmake

# D:/repos/JzRE/tests/CMakeLists.txt

# 查找 GTest

find_package(GTest REQUIRED)

# 添加子目录，每个子目录包含针对一个模块的测试

add_subdirectory(Core)
add_subdirectory(Scene)
```

```cmake

# D:/repos/JzRE/tests/Core/CMakeLists.txt

add_executable(Test_Core
test_logger.cpp
)

# 链接被测试的库 JzCore 和 GTest

target_link_libraries(Test_Core
PRIVATE
JzCore
GTest::gtest_main
)

# 添加测试

add_test(NAME Test.Core COMMAND Test_Core)
```

## 目录结构

### 方案一：扁平结构 (推荐用于本项目)

在这种模式下，头文件（`.h`）和源文件（`.cpp`）并列存放在模块的根目录下
。

文件结构:

```
src/
  └── Core/
      ├── CMakeLists.txt
      ├── JzLogger.h         # 公开头文件
      ├── JzLogger.cpp
      ├── internal_helper.h  # 内部使用的头文件
      └── internal_helper.cpp
```

CMakeLists.txt:

```cmake

# src/Core/CMakeLists.txt

add_library(JzCore STATIC)

target_sources(JzCore PRIVATE
    JzLogger.cpp
    internal_helper.cpp
)

# 关键点在这里：

# 将整个目录作为 PUBLIC include 目录。

# 这意味着其他模块可以 #include "JzLogger.h"

# 它们也可以 #include "internal_helper.h"，但这是一种约定俗成的“不应该”。

target_include_directories(JzCore PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
```

-   优点:
-   缺点: 这种模式是传统 C++ 库的标准做法，它在物理上隔离了公开接口和实现细节。

### 方案二：分离式组织

文件结构:

```
src/
  └── Core/
      ├── CMakeLists.txt
      ├── include/
      │   └── JzRE/Core/       # 加上命名空间路径，防止头文件冲突
      │       └── JzLogger.h   # 唯一的公开头文件
      └── src/
          ├── JzLogger.cpp
          ├── internal_helper.h  # 私有头文件
          └── internal_helper.cpp
```

CMakeLists.txt:

```cmake

# src/Core/CMakeLists.txt

add_library(JzCore STATIC)

target_sources(JzCore PRIVATE
    src/JzLogger.cpp
    src/internal_helper.cpp
)

# 关键点在这里：

# PUBLIC include 目录只指向 include 文件夹。

target_include_directories(JzCore
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
```

其他模块这样引用：`#include <JzRE/Core/JzLogger.h>`，它们无法访问到 `src` 目录下的 `internal_helper.h`。

-   优点:
-   缺点: 对于 JzRE 这样一个一体化开发的项目，所有模块都是为了同一个目标服务，而不是为了独立发布。因此，方案一（扁平结构）的简洁性和敏捷性优势更加突出。它减少了不必要的目录层级和配置复杂性，让开发者能更专注于代码本身。

你可以通过编码约定来弥补其“接口不明确”的缺点：

1.  在文件头部注释中明确指出头文件是 @public 还是 @private。
2.  将内部使用的头文件命名为 `*_internal.h` 或 `*_p.h` (private) 来进行区分。

## 跨平台代码组织

### 方案一：集中式平台模块

种结构试图将所有平台相关代码集中到一个地方，形成一个平台抽象层（Platform Abstraction Layer, PAL）。

```
src/
  ├── Core/
  ├── Platform/           # 新增：跨平台适配层
  │   ├── CMakeLists.txt
  │   ├── Common/         # 平台无关的通用实现
  │   ├── Windows/
  │   ├── Linux/
  │   ├── macOS/
  │   └── Android/
  ├── RHI/
  ├── Render/
  └── ...
```

-   优点：
    -   平台相关代码集中管理，便于维护和移植
    -   清晰的抽象边界，避免平台代码污染业务逻辑
    -   便于条件编译和构建系统管理
-   缺点：某些模块可能需要频繁与平台层交互

### 方案二：分散到各模块内

在每个模块内部处理平台差异：

```
src/
  ├── Core/
  │   ├── Platform/  <-- 只放 Core 模块需要的平台代码
  │   │   ├── JzThreading_Windows.cpp
  │   │   └── JzThreading_Linux.cpp
  │   ├── JzThreading.h
  │   └── JzThreading.cpp (平台无关的逻辑)
  ├── UI/
  │   ├── Platform/  <-- 只放 UI 模块需要的平台代码
  │   │   ├── JzFileDialog_Windows.cpp
  │   │   └── JzFileDialog_Linux.cpp
  │   ├── JzFileDialog.h
  │   └── ...
  ├── Scene/
```

这种结构遵循高内聚、低耦合的原则。一个模块应该包含实现其功能所需的所有东西，包括平台相关的实现细节。一个简单的原则是：平台相关的代码，应该和它所实现的、平台无关的接口放在同一个模块里。

方案二是现代大型跨平台项目（如 Unreal Engine, Godot 等）普遍采用的策略，因为它能更好地管理复杂性，并保持一个清晰、无循环的依赖图。
