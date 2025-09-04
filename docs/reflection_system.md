# JzRE 反射系统

本文档介绍 JzRE 反射系统的使用方法和实现细节。

## 概述

JzRE 反射系统是一个类似于 Unreal Engine 的 C++反射系统，提供运行时类型信息、属性访问和方法调用功能。

## 核心组件

### 1. JzREHeaderTool

基于 Clang AST 的代码生成工具，自动分析 C++代码并生成反射信息。

**功能特性：**

-   5 阶段处理流程：预处理 → 解析 → 提取 → 验证 → 生成
-   支持类、属性、方法的自动反射
-   生成类型安全的访问器代码
-   集成到 CMake 构建流程

### 2. 反射宏系统

提供易用的宏接口来标记需要反射的代码元素。

```cpp
// 类反射
JzRE_CLASS(Category="MyCategory")
class MyClass {
    JzRE_GENERATED_BODY()

    // 属性反射
    JzRE_PROPERTY(EditAnywhere, Category="Stats")
    float health;

    // 方法反射
    JzRE_FUNCTION(Category="Actions")
    void DoSomething();
};
```

### 3. 运行时反射 API

提供完整的运行时类型信息访问接口。

## 使用指南

### 1. 标记反射类

使用`JzRE_CLASS`宏标记需要反射的类：

```cpp
#include "JzReflectable.h"

JzRE_CLASS(Category="Gameplay")
class JzRE_CLASS(Category="Gameplay") MyActor {
    JzRE_GENERATED_BODY()

public:
    MyActor() : health(100.0f), name("Default") {}

    // 公共属性和方法
};
```

### 2. 标记反射属性

使用`JzRE_PROPERTY`宏标记需要反射的成员变量：

```cpp
// 基本属性
JzRE_PROPERTY(EditAnywhere)
float health;

// 带分类的属性
JzRE_PROPERTY(EditAnywhere, Category="Stats")
int experience;

// 只读属性
JzRE_PROPERTY(VisibleAnywhere, Category="Info")
std::string name;
```

**支持的属性标志：**

-   `EditAnywhere` - 可在任何地方编辑
-   `VisibleAnywhere` - 在任何地方可见
-   `Category="名称"` - 分类显示

### 3. 标记反射方法

使用`JzRE_FUNCTION`宏标记需要反射的成员函数：

```cpp
// 基本方法
JzRE_FUNCTION()
void DoSomething();

// 带分类的方法
JzRE_FUNCTION(Category="Actions")
void TakeDamage(float amount);

// 编辑器可调用方法
JzRE_FUNCTION(CallInEditor="true", Category="Tools")
void ResetValues();
```

### 4. CMake 集成

在 CMakeLists.txt 中使用反射代码生成：

```cmake
# 生成反射代码
generate_reflection_code(MyTarget
    HEADER_FILES
        MyClass.h
        AnotherClass.h
    INCLUDE_PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include
)

# 添加生成的文件到目标
add_executable(MyTarget
    main.cpp
    ${MyTarget_GENERATED_HEADER}
    ${MyTarget_GENERATED_SOURCE}
)

# 添加依赖
add_dependencies(MyTarget ${MyTarget_REFLECTION_TARGET})
```

### 5. 运行时使用

```cpp
#include "JzReflectable.h"
#include "JzRE.generated.h"

int main() {
    auto& registry = JzRE::JzReflectionRegistry::GetInstance();

    // 创建实例
    auto* obj = registry.CreateInstance<MyActor>("MyActor");

    // 获取类信息
    const auto* classInfo = registry.GetClassInfo("MyActor");

    // 访问属性
    const auto* healthField = classInfo->GetField("health");
    if (healthField) {
        // 获取值
        auto value = healthField->getter(obj);
        float health = std::any_cast<float>(value);

        // 设置值
        healthField->setter(obj, 50.0f);
    }

    // 查询方法
    const auto* method = classInfo->GetMethod("TakeDamage");
    if (method) {
        std::cout << "Found method: " << method->name << std::endl;
    }

    delete obj;
    return 0;
}
```

## 高级特性

### 1. 继承支持

反射系统完全支持类继承：

```cpp
JzRE_CLASS(Category="Base")
class BaseClass {
    JzRE_GENERATED_BODY()

    JzRE_PROPERTY(EditAnywhere)
    float baseValue;
};

JzRE_CLASS(Category="Derived")
class DerivedClass : public BaseClass {
    JzRE_GENERATED_BODY()

    JzRE_PROPERTY(EditAnywhere)
    int derivedValue;
};
```

### 2. 元数据系统

所有反射元素都支持自定义元数据：

```cpp
JzRE_PROPERTY(EditAnywhere, Category="Stats", Min="0", Max="100")
float health;

JzRE_FUNCTION(Category="Actions", Tooltip="造成伤害给角色")
void TakeDamage(float damage);
```

### 3. 类型安全

反射系统在编译时和运行时都提供类型安全保证：

```cpp
// 编译时类型检查
auto* typedObj = registry.CreateInstance<MyActor>("MyActor");

// 运行时类型转换保护
try {
    float value = std::any_cast<float>(fieldValue);
} catch (const std::bad_any_cast& e) {
    // 处理类型错误
}
```

## 构建要求

-   CMake 3.15+
-   C++20 编译器
-   LLVM/Clang 17.0+（通过 vcpkg 安装）
-   vcpkg 包管理器

## 构建步骤

1. 确保 vcpkg 已安装并配置
2. 运行 CMake 配置：
    ```bash
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[vcpkg根目录]/scripts/buildsystems/vcpkg.cmake
    ```
3. 构建项目：
    ```bash
    cmake --build build
    ```

构建过程中，JzREHeaderTool 会自动运行并生成反射代码。

## 示例项目

参考`src/JzTestReflectable.h`和`src/JzTestReflectable.cpp`查看完整的使用示例。

## 故障排除

### 常见问题

1. **生成的反射代码无法找到**

    - 确保在 CMakeLists.txt 中正确配置了`generate_reflection_code`
    - 检查是否添加了依赖关系：`add_dependencies(target reflection_target)`

2. **Clang 找不到头文件**

    - 确保在`INCLUDE_PATHS`中包含所有必要的目录
    - 检查 vcpkg 的 LLVM 安装是否正确

3. **反射宏无法识别**

    - 确保包含了`JzReflectable.h`
    - 检查宏使用是否正确（注意大小写）

4. **运行时找不到类型**
    - 确保包含了生成的头文件`JzRE.generated.h`
    - 检查类名是否完全匹配（包括命名空间）

## 扩展开发

### 添加新的属性标志

1. 在`JzReflectable.h`中的`EJzPropertyFlags`枚举添加新标志
2. 在`JzREHeaderTool.cpp`的解析逻辑中添加处理代码
3. 更新代码生成模板

### 支持新的数据类型

1. 确保类型可以通过`std::any`存储
2. 在生成的 setter/getter 中添加适当的类型转换
3. 更新类型名称清理逻辑

### 添加序列化支持

反射系统为序列化提供了基础，可以基于类型信息实现：

-   JSON 序列化/反序列化
-   二进制序列化
-   自定义格式支持

## 性能考虑

-   反射信息在程序启动时一次性生成
-   运行时访问开销主要来自`std::any`转换
-   大量反射操作建议缓存类型信息指针
-   生成的代码经过优化，避免了重复计算

## 与 Unreal Engine 对比

| 特性     | JzRE 反射    | Unreal Engine |
| -------- | ------------ | ------------- |
| 代码生成 | ✅ Clang AST | ✅ UHT        |
| 属性反射 | ✅           | ✅            |
| 方法反射 | ✅           | ✅            |
| 继承支持 | ✅           | ✅            |
| 元数据   | ✅           | ✅            |
| 蓝图集成 | ❌           | ✅            |
| GC 支持  | ❌           | ✅            |
| 网络复制 | ❌           | ✅            |

JzRE 反射系统专注于核心反射功能，为进一步扩展提供了坚实的基础。
