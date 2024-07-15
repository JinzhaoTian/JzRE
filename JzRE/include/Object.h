#pragma once

#include "CommonTypes.h"
#include "Transform.h"
#include "GraphicsInterfaceRenderer.h"

namespace JzRE {
class Object {
public:
    virtual void Update(float deltaTime) = 0;
    virtual void Draw(GraphicsInterfaceRenderer &renderer) = 0;

protected:
    Transform transform;
};
} // namespace JzRE