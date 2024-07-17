#pragma once

#include "CommonTypes.h"
#include "RenderableObject.h"

namespace JzRE {
class GraphicsInterfaceScene {
public:
    GraphicsInterfaceScene();
    ~GraphicsInterfaceScene();

    void AddObject(SharedPtr<RenderableObject> object);
    void RemoveObject(SharedPtr<RenderableObject> object);

    // void AddLight(SharedPtr<Light> light);
    // void RemoveLight(SharedPtr<Light> light);

    // void SetCamera(SharedPtr<Camera> camera);
    // SharedPtr<Camera> GetCamera() const;

    void Update(F32 deltaTime);
    void Draw() const;

private:
    List<SharedPtr<RenderableObject>> objects;
    // List<SharedPtr<Light>> lights;
    // SharedPtr<Camera> camera;
};
} // namespace JzRE