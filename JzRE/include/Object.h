#pragma once

#include "CommonTypes.h"
#include "Transform.h"
#include "GraphicsInterfaceShader.h"

namespace JzRE {
class Object {
public:
    virtual void Update(F32 deltaTime) = 0;
    virtual void Draw() const = 0;

protected:
    Transform transform;
};
} // namespace JzRE