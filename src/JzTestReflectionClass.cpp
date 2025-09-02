#include "JzTestReflectionClass.h"
#include <sstream>

namespace JzRE {

JzTestReflectionClass::JzTestReflectionClass() {
    // 构造函数实现
}

void JzTestReflectionClass::TakeDamage(I32 damage) {
    health -= damage;
    if (health < 0) {
        health = 0;
        isActive = false;
    }
}

void JzTestReflectionClass::Heal(I32 amount) {
    health += amount;
    if (health > 0 && !isActive) {
        isActive = true;
    }
}

String JzTestReflectionClass::GetInfo() const {
    std::ostringstream oss;
    oss << "Player: " << playerName 
        << ", Health: " << health
        << ", Speed: " << movementSpeed
        << ", Active: " << (isActive ? "true" : "false")
        << ", XP: " << experiencePoints;
    return oss.str();
}

void JzTestReflectionClass::SetMovementSpeed(F32 speed) {
    if (speed >= 0.0f) {
        movementSpeed = speed;
    }
}

} // namespace JzRE
