#pragma once

#include "JzReflectable.h"
#include "SimpleReflectionTest.generated.h"

using namespace JzRE;

JzRE_CLASS()
class ATestActor
{
    GENERATED_BODY()

public:
    ATestActor() : health(100.0f), name("TestActor") {}

    JzRE_PROPERTY(EditAnywhere)
    float health;

    JzRE_PROPERTY(EditAnywhere, Category="Info")
    String name;

    JzRE_METHOD()
    void SetHealth(float newHealth) {
        health = newHealth;
    }

    JzRE_METHOD()
    String GetName() const {
        return name;
    }
};
