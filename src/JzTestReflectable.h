#pragma once

#include "JzReflectable.h"
#include "CommonTypes.h"
#include <string>

namespace JzRE {

// 测试反射类 - 演示基本属性反射
JzRE_CLASS(Category="Test")
class JzRE_CLASS(Category="Test") JzTestActor {
public:
    JzRE_GENERATED_BODY()
    
    JzTestActor() : health(100.0f), speed(5.0f), name("DefaultActor"), isActive(true) {}
    
    // 基本属性
    JzRE_PROPERTY(EditAnywhere, Category="Stats")
    float health;
    
    JzRE_PROPERTY(EditAnywhere, Category="Stats") 
    float speed;
    
    JzRE_PROPERTY(EditAnywhere, Category="Basic")
    std::string name;
    
    JzRE_PROPERTY(VisibleAnywhere, Category="Basic")
    bool isActive;
    
    // 测试方法
    JzRE_FUNCTION(CallInEditor="true", Category="Actions")
    void TakeDamage(float damage);
    
    JzRE_FUNCTION(Category="Actions")
    void SetName(const std::string& newName);
    
    JzRE_FUNCTION(Category="Query") 
    bool IsAlive() const;
    
    JzRE_FUNCTION(Category="Query")
    float GetHealthPercentage() const;

private:
    // 私有方法不会被反射
    void InternalUpdate() {}
};

// 继承测试
JzRE_CLASS(Category="Test")
class JzRE_CLASS(Category="Test") JzTestPlayer : public JzTestActor {
public:
    JzRE_GENERATED_BODY()
    
    JzTestPlayer() : experience(0), level(1) {}
    
    JzRE_PROPERTY(EditAnywhere, Category="Player")
    int experience;
    
    JzRE_PROPERTY(VisibleAnywhere, Category="Player")
    int level;
    
    JzRE_FUNCTION(Category="Player")
    void GainExperience(int amount);
    
    JzRE_FUNCTION(Category="Player")
    void LevelUp();
};

// 简单数据结构测试
JzRE_CLASS(Category="Data")
struct JzRE_CLASS(Category="Data") JzTestVector3 {
    JzRE_GENERATED_BODY()
    
    JzTestVector3() : x(0.0f), y(0.0f), z(0.0f) {}
    JzTestVector3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    JzRE_PROPERTY(EditAnywhere)
    float x;
    
    JzRE_PROPERTY(EditAnywhere)
    float y;
    
    JzRE_PROPERTY(EditAnywhere)
    float z;
    
    JzRE_FUNCTION()
    float Length() const;
    
    JzRE_FUNCTION()
    void Normalize();
};

} // namespace JzRE
