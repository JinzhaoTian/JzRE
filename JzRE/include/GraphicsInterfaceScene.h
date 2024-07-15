#pragma once

#include "CommonTypes.h"
#include "Object.h"
#include "GraphicsInterfaceShader.h"

namespace JzRE {
class GraphicsInterfaceScene {
public:
    void AddObject(RawPtr<Object> object);
    void RemoveObject(RawPtr<Object> object);
    void Update(float deltaTime);
    void Draw(GraphcsInterfaceShader &shader);

private:
    List<RawPtr<Object>> objects;
};
} // namespace JzRE