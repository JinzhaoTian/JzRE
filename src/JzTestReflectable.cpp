#include "JzTestReflectable.h"
#include <cmath>

namespace JzRE {

// JzTestActor 实现
void JzTestActor::TakeDamage(float damage) {
    health -= damage;
    if (health < 0.0f) {
        health = 0.0f;
        isActive = false;
    }
}

void JzTestActor::SetName(const std::string& newName) {
    name = newName;
}

bool JzTestActor::IsAlive() const {
    return health > 0.0f;
}

float JzTestActor::GetHealthPercentage() const {
    return health / 100.0f;
}

// JzTestPlayer 实现
void JzTestPlayer::GainExperience(int amount) {
    experience += amount;
    // 简单的升级逻辑
    while (experience >= level * 100) {
        LevelUp();
    }
}

void JzTestPlayer::LevelUp() {
    level++;
    health += 10.0f; // 升级时增加生命值
}

// JzTestVector3 实现
float JzTestVector3::Length() const {
    return std::sqrt(x * x + y * y + z * z);
}

void JzTestVector3::Normalize() {
    float length = Length();
    if (length > 0.0f) {
        x /= length;
        y /= length;
        z /= length;
    }
}

} // namespace JzRE
