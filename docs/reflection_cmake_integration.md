# JzRE 反射系统 CMake 集成指南

## 概述

该文档介绍了改进后的 JzRE 反射系统，该系统已完全集成到 CMake 构建流程中，无需手动运行 Python 脚本。反射工具使用 libclang 进行完整的 AST 解析，提供更准确和可靠的反射代码生成。

## 主要改进

### 1. 完整的 libclang AST 解析

-   自动检测和配置 libclang 库
-   跨平台支持（Windows、macOS、Linux）
-   准确解析复杂的 C++ 语法结构
-   回退到正则表达式解析（如果 libclang 不可用）

### 2. 智能 CMake 集成

-   自动检测包含反射宏的头文件
-   增量构建支持（只重新生成修改过的文件）
-   精细的依赖管理
-   并行构建支持

### 3. 自动化工作流程

-   编译前自动生成反射代码
-   无需手动干预
-   错误处理和验证
-   详细的构建日志

## 使用方法

### 基本构建流程

```bash
# 1. 配置项目
cmake -B build -S .

# 2. 构建项目（会自动生成反射代码）
cmake --build build

# 3. 运行程序
./build/bin/JzRE
```

### 开发者工具

```bash
# 手动运行反射工具（用于调试）
cmake --build build --target run_reflection_tool

# 强制重新生成所有反射代码
cmake --build build --target regenerate_reflection_code

# 验证反射代码
cmake --build build --target verify_reflection_code

# 清理生成的反射文件
cmake --build build --target clean_reflection_code
```

## 依赖要求

### 必需依赖

-   Python 3.7+
-   CMake 3.15+

### 可选依赖（推荐）

-   libclang (通过 `pip install libclang` 安装)

如果没有 libclang，系统会自动回退到正则表达式解析模式。

## 反射宏使用示例

### 基本类定义

```cpp
// MyComponent.h

#pragma once
#include "JzObject.h"

// 类级别反射宏
JZCLASS(meta=(DisplayName="我的组件", Category="Components"))
class MyComponent : public JzObject
{
private:
    // 属性反射宏
    JZPROPERTY(meta=(DisplayName="位置", Category="Transform", Tooltip="组件的3D位置"))
    glm::vec3 position = glm::vec3(0.0f);

    JZPROPERTY(meta=(DisplayName="是否可见", Category="Rendering"))
    bool visible = true;

public:
    // 方法反射宏
    JZMETHOD(meta=(DisplayName="设置位置", Category="Transform"))
    void SetPosition(const glm::vec3& newPos);

    JZMETHOD(meta=(DisplayName="获取位置"))
    glm::vec3 GetPosition() const;

    // 声明反射支持
    JZREFLECTION_FILE()
};
```

### 实现文件

```cpp
// MyComponent.cpp

#include "MyComponent.h"
#include "MyComponent.generated.h"  // 自动生成的反射代码

void MyComponent::SetPosition(const glm::vec3& newPos)
{
    position = newPos;
}

glm::vec3 MyComponent::GetPosition() const
{
    return position;
}
```

## 生成的文件结构

对于每个包含反射宏的头文件 `MyComponent.h`，系统会生成：

```
build/generated/
├── MyComponent.generated.h    # 反射声明
└── MyComponent.generated.cpp  # 反射实现
```

### 生成文件内容示例

#### MyComponent.generated.h

```cpp
// 自动生成的反射代码 - 请勿手动编辑
#pragma once

#include "JzObject.h"
#include "JzReflectable.h"

namespace JzRE {

// 类 MyComponent 的反射信息声明
class MyComponentReflectionData {
public:
    static void RegisterReflectionData();
    static JzClass* GetStaticClass();
};

} // namespace JzRE

// 类的反射声明宏实现
#define IMPLEMENT_MYCOMPONENT_REFLECTION() \
    // ... 宏定义内容
```

#### MyComponent.generated.cpp

```cpp
// 自动生成的反射代码 - 请勿手动编辑
#include "MyComponent.generated.h"
#include "MyComponent.h"

namespace JzRE {

void MyComponentReflectionData::RegisterReflectionData() {
    // 注册类信息、属性和方法
    // ... 自动生成的注册代码
}

} // namespace JzRE
```

## 构建配置选项

### CMake 变量

-   `REFLECTION_USE_AST`: 是否使用 libclang AST 解析（自动检测）
-   `REFLECTION_OUTPUT_DIRECTORY`: 生成文件的输出目录
-   `CMAKE_VERBOSE_MAKEFILE`: 启用详细的构建日志

### 环境变量

-   `LIBCLANG_PATH`: 手动指定 libclang 库路径（可选）

## 故障排除

### 常见问题

1. **libclang 未找到**

    ```
    警告: libclang不可用，将回退到正则表达式解析
    ```

    - 解决方法：`pip install libclang`

2. **反射文件未生成**

    - 检查头文件是否包含反射宏
    - 使用 `--verbose` 标志查看详细日志
    - 运行 `verify_reflection_code` 目标检查

3. **编译错误：找不到 .generated.h 文件**
    - 确保构建顺序正确
    - 检查 `generate_reflection_code_target` 依赖是否正确设置

### 调试方法

1. **启用详细日志**

    ```bash
    cmake --build build --verbose
    ```

2. **手动运行反射工具**

    ```bash
    python3 tools/reflection_tool.py --source-dir src --output-dir build/generated --verbose
    ```

3. **检查生成的文件**
    ```bash
    ls -la build/generated/
    ```

## 性能优化

### 增量构建

系统支持智能增量构建：

-   只重新生成修改过的文件
-   基于文件时间戳的依赖检测
-   并行处理多个文件

### 构建时间对比

| 模式         | 首次构建 | 增量构建 | AST 解析 |
| ------------ | -------- | -------- | -------- |
| 正则表达式   | 快       | 快       | 无       |
| libclang AST | 中等     | 快       | 精确     |

## 最佳实践

### 代码组织

1. 每个头文件只包含一个主要的反射类
2. 使用有意义的元数据属性
3. 保持反射宏和实际声明的接近

### 元数据设计

```cpp
// 推荐的元数据使用
JZPROPERTY(meta=(
    DisplayName="用户友好名称",
    Category="逻辑分组",
    Tooltip="详细说明"
))
```

### 构建脚本

```bash
#!/bin/bash
# build.sh - 推荐的构建脚本

set -e

echo "配置构建..."
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release

echo "构建项目..."
cmake --build build --parallel $(nproc)

echo "验证反射代码..."
cmake --build build --target verify_reflection_code

echo "构建完成！"
```

## 扩展性

### 添加新的反射宏

1. 在 `reflection_tool.py` 中添加新的正则表达式模式
2. 在 AST 解析器中添加对应的处理逻辑
3. 更新代码生成模板

### 自定义元数据属性

系统支持任意的元数据属性，只需在宏中声明即可：

```cpp
JZPROPERTY(meta=(
    CustomAttribute="自定义值",
    MinValue="0",
    MaxValue="100"
))
int customProperty;
```

## 结论

改进后的反射系统提供了：

-   完全自动化的工作流程
-   高性能的增量构建
-   准确的 AST 解析
-   易于使用的开发体验

系统已准备好投入生产使用，无需任何手动干预即可生成高质量的反射代码。
