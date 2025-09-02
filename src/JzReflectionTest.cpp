#include "JzReflectionTest.h"
#include "JzTestReflectionClass.h"
#include "JzObject.h"
#include <iostream>
#include <iomanip>

namespace JzRE {

void JzReflectionTest::RunAllTests() {
    std::cout << "=== JzRE 反射系统测试 ===\n" << std::endl;
    
    TestBasicReflection();
    TestPropertyAccess();
    TestReflectionRegistry();
    TestTypeInfo();
    
    std::cout << "=== 反射测试完成 ===\n" << std::endl;
}

void JzReflectionTest::TestBasicReflection() {
    std::cout << "--- 基础反射测试 ---" << std::endl;
    
    // 创建测试对象
    JzTestReflectionClass testObj;
    
    // 测试类信息获取
    auto classInfo = testObj.GetClass();
    if (classInfo) {
        std::cout << "✓ 类名: " << classInfo->GetName() << std::endl;
        std::cout << "✓ 父类: " << classInfo->GetParentName() << std::endl;
    } else {
        std::cout << "✗ 无法获取类信息" << std::endl;
        return;
    }
    
    // 测试属性数量
    auto properties = classInfo->GetAllProperties();
    std::cout << "✓ 找到 " << properties.size() << " 个反射属性" << std::endl;
    
    // 测试方法数量  
    auto methods = classInfo->GetAllMethods();
    std::cout << "✓ 找到 " << methods.size() << " 个反射方法" << std::endl;
    
    std::cout << std::endl;
}

void JzReflectionTest::TestPropertyAccess() {
    std::cout << "--- 属性访问测试 ---" << std::endl;
    
    JzTestReflectionClass testObj;
    auto classInfo = testObj.GetClass();
    
    if (!classInfo) {
        std::cout << "✗ 无法获取类信息" << std::endl;
        return;
    }
    
    // 测试属性读取
    std::cout << "属性读取测试:" << std::endl;
    auto properties = classInfo->GetAllProperties();
    for (const auto* property : properties) {
        String value = property->GetValue(&testObj);
        std::cout << "  " << property->GetName() << " = " << value 
                  << " (类型: " << property->GetTypeName() << ")" << std::endl;
    }
    
    // 测试属性设置
    std::cout << "\n属性设置测试:" << std::endl;
    
    auto healthProp = classInfo->GetProperty("health");
    if (healthProp) {
        std::cout << "  设置前 health = " << healthProp->GetValue(&testObj) << std::endl;
        Bool result = healthProp->SetValue(&testObj, "75");
        std::cout << "  设置 health = 75: " << (result ? "成功" : "失败") << std::endl;
        std::cout << "  设置后 health = " << healthProp->GetValue(&testObj) << std::endl;
    }
    
    auto nameProp = classInfo->GetProperty("playerName");
    if (nameProp) {
        std::cout << "  设置前 playerName = " << nameProp->GetValue(&testObj) << std::endl;
        Bool result = nameProp->SetValue(&testObj, "TestPlayer");
        std::cout << "  设置 playerName = TestPlayer: " << (result ? "成功" : "失败") << std::endl;
        std::cout << "  设置后 playerName = " << nameProp->GetValue(&testObj) << std::endl;
    }
    
    std::cout << std::endl;
}

void JzReflectionTest::TestReflectionRegistry() {
    std::cout << "--- 反射注册表测试 ---" << std::endl;
    
    auto& registry = JzReflectionRegistry::Get();
    
    // 获取所有注册的类
    auto classNames = registry.GetAllClassNames();
    std::cout << "已注册的类:" << std::endl;
    for (const auto& className : classNames) {
        auto classInfo = registry.GetClass(className);
        if (classInfo) {
            std::cout << "  - " << className 
                      << " (属性: " << classInfo->GetAllProperties().size() 
                      << ", 方法: " << classInfo->GetAllMethods().size() << ")" << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void JzReflectionTest::TestTypeInfo() {
    std::cout << "--- 类型信息测试 ---" << std::endl;
    
    JzTestReflectionClass testObj;
    
    // 测试类型检查
    Bool isJzObject = testObj.IsA<JzObject>();
    Bool isTestClass = testObj.IsA<JzTestReflectionClass>();
    
    std::cout << "类型检查:" << std::endl;
    std::cout << "  是 JzObject: " << (isJzObject ? "是" : "否") << std::endl;
    std::cout << "  是 JzTestReflectionClass: " << (isTestClass ? "是" : "否") << std::endl;
    
    // 测试类型转换
    JzObject* basePtr = &testObj;
    auto* castPtr = basePtr->Cast<JzTestReflectionClass>();
    
    std::cout << "类型转换:" << std::endl;
    std::cout << "  基类指针转换为派生类: " << (castPtr != nullptr ? "成功" : "失败") << std::endl;
    
    if (castPtr) {
        std::cout << "  转换后访问属性 health: " << castPtr->health << std::endl;
    }
    
    std::cout << std::endl;
}

void JzReflectionTest::PrintPropertyInfo(const JzProperty* property) {
    if (!property) return;
    
    const auto& metadata = property->GetMetadata();
    
    std::cout << "属性信息:" << std::endl;
    std::cout << "  名称: " << property->GetName() << std::endl;
    std::cout << "  显示名称: " << metadata.GetDisplayName() << std::endl;
    std::cout << "  分类: " << metadata.GetCategory() << std::endl;
    std::cout << "  提示: " << metadata.GetTooltip() << std::endl;
    std::cout << "  类型: " << property->GetTypeName() << std::endl;
    std::cout << "  可读: " << (property->IsReadable() ? "是" : "否") << std::endl;
    std::cout << "  可写: " << (property->IsWritable() ? "是" : "否") << std::endl;
}

} // namespace JzRE
