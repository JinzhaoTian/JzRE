#include "JzRenderEngine.h"
#include "JzREHub.h"
#include "JzTestReflectable.h"
#include "JzReflectable.h"
#include "JzRE.generated.h"
#include <iostream>

void TestReflectionSystem() {
    std::cout << "=== JzRE Reflection System Test ===" << std::endl;
    
    // 测试反射注册表
    auto& registry = JzRE::JzReflectionRegistry::GetInstance();
    
    std::cout << "\nRegistered Classes:" << std::endl;
    registry.PrintAllClasses();
    
    // 测试运行时类型创建
    std::cout << "\n=== Runtime Type Creation Test ===" << std::endl;
    
    // 创建JzTestActor实例
    auto* actor = registry.CreateInstance<JzRE::JzTestActor>("JzRE::JzTestActor");
    if (actor) {
        std::cout << "Successfully created JzTestActor instance" << std::endl;
        
        // 获取类信息
        const auto* classInfo = registry.GetClassInfo("JzRE::JzTestActor");
        if (classInfo) {
            std::cout << "Class: " << classInfo->GetName() << std::endl;
            std::cout << "Size: " << classInfo->GetSize() << " bytes" << std::endl;
            
            // 测试属性访问
            const auto* healthField = classInfo->GetField("health");
            if (healthField) {
                std::cout << "Field 'health' found: " << healthField->type << std::endl;
                
                // 测试getter
                try {
                    auto healthValue = healthField->getter(actor);
                    float health = std::any_cast<float>(healthValue);
                    std::cout << "Current health: " << health << std::endl;
                    
                    // 测试setter
                    healthField->setter(actor, 50.0f);
                    healthValue = healthField->getter(actor);
                    health = std::any_cast<float>(healthValue);
                    std::cout << "Health after setting to 50: " << health << std::endl;
                } catch (const std::exception& e) {
                    std::cout << "Error accessing health field: " << e.what() << std::endl;
                }
            }
            
            // 测试方法信息
            const auto* takeDamageMethod = classInfo->GetMethod("TakeDamage");
            if (takeDamageMethod) {
                std::cout << "Method 'TakeDamage' found: " << takeDamageMethod->returnType 
                         << " (parameters: " << takeDamageMethod->parameters.size() << ")" << std::endl;
            }
        }
        
        // 清理
        delete actor;
    } else {
        std::cout << "Failed to create JzTestActor instance" << std::endl;
    }
    
    // 测试继承
    std::cout << "\n=== Inheritance Test ===" << std::endl;
    auto* player = registry.CreateInstance<JzRE::JzTestPlayer>("JzRE::JzTestPlayer");
    if (player) {
        std::cout << "Successfully created JzTestPlayer instance" << std::endl;
        
        const auto* playerClassInfo = registry.GetClassInfo("JzRE::JzTestPlayer");
        if (playerClassInfo) {
            std::cout << "Player class base classes: ";
            for (const auto& baseClass : playerClassInfo->baseClasses) {
                std::cout << baseClass << " ";
            }
            std::cout << std::endl;
        }
        
        delete player;
    }
    
    std::cout << "\n=== Reflection System Test Complete ===" << std::endl;
}

int main()
{
    std::cout << "Starting JzRE..." << std::endl;
    
    // 测试反射系统
    TestReflectionSystem();
    
    std::cout << "\nStarting Render Engine..." << std::endl;
    
    // JzRE::JzREHub hub;
    // hub.Run();

    JzRE::JzRenderEngine re;
    re.Run();

    return 0;
}