#pragma once

#include "CommonTypes.h"
#include "RenderableObject.h"
#include "GraphicsInterfaceShader.h"
#include "GraphicsInterfaceCamera.h"

namespace JzRE {
class GraphicsInterfaceScene {
public:
    GraphicsInterfaceScene();
    ~GraphicsInterfaceScene();

    void AddObject(SharedPtr<RenderableObject> object);
    void RemoveObject(SharedPtr<RenderableObject> object);

    // void AddLight(SharedPtr<Light> light);
    // void RemoveLight(SharedPtr<Light> light);

    void SetCamera(SharedPtr<GraphicsInterfaceCamera> camera);
    SharedPtr<GraphicsInterfaceCamera> GetCamera() const;

    void Update(F32 deltaTime);
    void Draw() const;

private:
    List<SharedPtr<RenderableObject>> objects;
    // List<SharedPtr<Light>> lights;
    SharedPtr<GraphicsInterfaceCamera> camera;
};
} // namespace JzRE