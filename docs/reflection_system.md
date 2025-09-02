# JzRE 反射系统使用指南

## 概述

JzRE 反射系统是一套类似于 UnrealHeaderTool 的 C++ 反射工具，可以在运行时获取类型信息、访问属性和调用方法。该系统使用 Python + libclang + CMake 构建，并完全集成到项目的构建流程中。

## 系统架构

```
C++ 源文件 -> 反射宏 -> Python 工具 -> libclang 解析 -> 生成代码 -> 编译链接
```

### 主要组件

1. **JzObject 基类**: 所有可反射类的基类
2. **反射宏**: JZCLASS、JZPROPERTY、JZMETHOD 等标记宏
3. **Python 反射工具**: 代码生成器
4. **反射注册中心**: 运行时类型数据库
5. **CMake 集成**: 自动化构建流程

## 使用方法

### 1. 创建可反射的类

```cpp
#pragma once

#include "JzObject.h"
#include "JzReflectable.h"

// 标记文件包含反射代码（可选）
JZREFLECTION_FILE()

namespace JzRE {

// 使用 JZCLASS 宏标记类
JZCLASS(meta=(DisplayName="My Game Object", Category="Game"))
class JzMyGameObject : public JzObject {
public:
    JzMyGameObject() = default;

    // 使用 JZPROPERTY 标记属性
    JZPROPERTY(meta=(DisplayName="Health Points", Category="Stats", Tooltip="Character health"))
    I32 health = 100;

    JZPROPERTY(meta=(DisplayName="Player Name", Category="Identity"))
    String playerName = "DefaultPlayer";

    JZPROPERTY(meta=(DisplayName="Movement Speed", Category="Movement"))
    F32 movementSpeed = 5.0f;

    // 使用 JZMETHOD 标记方法
    JZMETHOD(meta=(DisplayName="Take Damage", Category="Combat"))
    void TakeDamage(I32 damage) {
        health -= damage;
        if (health < 0) health = 0;
    }

    JZMETHOD(meta=(DisplayName="Get Info", Category="Utility"))
    String GetInfo() const {
        return "Name: " + playerName + ", Health: " + std::to_string(health);
    }

private:
    // 私有成员不会被反射
    I32 m_internalData = 0;
};

} // namespace JzRE
```

### 2. 元数据语法

反射宏支持丰富的元数据定义：

```cpp
// 类元数据
JZCLASS(meta=(DisplayName="显示名称", Category="分类"))

// 属性元数据
JZPROPERTY(meta=(DisplayName="显示名称", Category="分类", Tooltip="提示信息"))

// 方法元数据
JZMETHOD(meta=(DisplayName="显示名称", Category="分类"))
```

### 3. 运行时访问反射信息

```cpp
#include "JzMyGameObject.h"

void TestReflection() {
    JzMyGameObject obj;

    // 获取类信息
    auto* classInfo = obj.GetClass();
    std::cout << "类名: " << classInfo->GetName() << std::endl;

    // 访问属性
    auto* healthProp = classInfo->GetProperty("health");
    if (healthProp) {
        // 读取属性值
        String value = healthProp->GetValue(&obj);
        std::cout << "Health: " << value << std::endl;

        // 设置属性值
        healthProp->SetValue(&obj, "75");
        std::cout << "New Health: " << healthProp->GetValue(&obj) << std::endl;
    }

    // 获取所有属性
    auto properties = classInfo->GetAllProperties();
    for (const auto* prop : properties) {
        std::cout << "属性: " << prop->GetName()
                  << " = " << prop->GetValue(&obj)
                  << " (类型: " << prop->GetTypeName() << ")" << std::endl;
    }

    // 类型检查
    if (obj.IsA<JzObject>()) {
        std::cout << "obj 是 JzObject 的实例" << std::endl;
    }

    // 类型转换
    JzObject* basePtr = &obj;
    auto* derivedPtr = basePtr->Cast<JzMyGameObject>();
    if (derivedPtr) {
        std::cout << "类型转换成功" << std::endl;
    }
}
```

## 构建流程

### 1. CMake 配置

项目已经自动配置好 CMake 集成，构建时会：

1. 扫描源文件中的反射宏
2. 调用 Python 反射工具生成代码
3. 将生成的代码编译到项目中

### 2. 手动运行反射工具

```bash
# 在项目根目录运行
python3 tools/reflection_tool.py --source-dir src --output-dir build/generated --verbose
```

### 3. 构建项目

```bash
mkdir build && cd build
cmake ..
make
```

## 支持的类型

### 基础类型

-   `Bool` - 布尔值
-   `I32`, `U32`, `I64`, `U64` - 整数类型
-   `F32`, `F64` - 浮点类型
-   `String` - 字符串类型

### 扩展类型支持

可以通过模板特化添加对自定义类型的支持：

```cpp
// 在 JzObject.cpp 中添加新类型的实例化
INSTANTIATE_PROPERTY_ACCESSOR(JzMyGameObject, MyCustomType)
```

## API 参考

### JzObject 基类

```cpp
class JzObject {
public:
    // 获取类的反射信息
    virtual JzClass* GetClass() const;

    // 类型检查
    template<typename T> Bool IsA() const;

    // 类型转换
    template<typename T> T* Cast();

    // 属性访问
    String GetPropertyValue(const String& propertyName) const;
    Bool SetPropertyValue(const String& propertyName, const String& value);
    std::vector<String> GetPropertyNames() const;
};
```

### JzClass 反射信息

```cpp
class JzClass {
public:
    const String& GetName() const;
    const String& GetParentName() const;

    const JzProperty* GetProperty(const String& name) const;
    const JzMethod* GetMethod(const String& name) const;

    std::vector<const JzProperty*> GetAllProperties() const;
    std::vector<const JzMethod*> GetAllMethods() const;
};
```

### JzProperty 属性信息

```cpp
class JzProperty {
public:
    const String& GetName() const;
    const JzPropertyMetadata& GetMetadata() const;

    String GetValue(const JzObject* object) const;
    Bool SetValue(JzObject* object, const String& value) const;

    String GetTypeName() const;
    Bool IsReadable() const;
    Bool IsWritable() const;
};
```

## 限制和注意事项

1. **继承限制**: 所有可反射类必须继承自 `JzObject`
2. **宏位置**: 反射宏必须放在 `public:` 访问级别
3. **类型支持**: 目前仅支持基础数据类型，复杂类型需要扩展
4. **性能考虑**: 反射访问比直接访问慢，不适合频繁调用
5. **构建依赖**: 需要 Python 3 和 libclang（可选）

## 故障排除

### 常见问题

1. **生成代码未找到**: 检查 CMake 是否正确配置了依赖关系
2. **Python 工具执行失败**: 确保 Python 3 可用并安装了必要的依赖
3. **反射信息为空**: 检查类是否正确继承 JzObject 并使用了反射宏
4. **编译错误**: 确保包含了必要的头文件和生成的代码文件

### 调试方法

1. 使用 `JZREFLECTION_DEBUG_INFO` 宏输出调试信息
2. 检查生成的 `.generated.h` 和 `.generated.cpp` 文件
3. 运行 `JzReflectionTest::RunAllTests()` 验证系统功能

## 扩展功能

### 计划中的功能

1. **函数反射**: 动态调用反射方法
2. **序列化支持**: 基于反射的对象序列化
3. **编辑器集成**: 属性面板自动生成
4. **蓝图系统**: 可视化脚本支持

### 自定义扩展

可以通过以下方式扩展反射系统：

1. 添加新的属性访问器类型
2. 扩展元数据类型
3. 添加新的反射宏
4. 自定义代码生成器

## 示例项目

参见 `src/JzTestReflectionClass.h` 和 `src/JzReflectionTest.cpp` 了解完整的使用示例。
