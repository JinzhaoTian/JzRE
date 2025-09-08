#pragma once

#include "JzReflectable.h"

#include "SimpleReflectionTest.generated.h"

// 最简单的反射测试
JzRE_CLASS()
class ATestActor
{
    GENERATED_BODY()

public:
    ATestActor() : health(100.0f) {}

    JzRE_PROPERTY(EditAnywhere)
    float health;

    JzRE_PROPERTY(EditAnywhere, Category="Info")
    int level;

    JzRE_METHOD()
    void SetHealth(float newHealth) {
        health = newHealth;
    }

    JzRE_METHOD()
    float GetHealth() const {
        return health;
    }
};
