#pragma once

#include "CommonTypes.h"
#include "RenderableObject.h"
#include "GraphicsInterfaceModel.h"
#include "GraphicsInterfaceShader.h"
#include "GraphicsInterfaceCamera.h"
#include "GraphicsInterfaceLight.h"

namespace JzRE {
class GraphicsInterfaceScene {
public:
    GraphicsInterfaceScene();
    ~GraphicsInterfaceScene();

    void AddObject(SharedPtr<RenderableObject> object);
    void RemoveObject(SharedPtr<RenderableObject> object);
    List<SharedPtr<RenderableObject>> GetObjects() const;

    void AddModel(SharedPtr<GraphicsInterfaceModel> object);
    void RemoveModel(SharedPtr<GraphicsInterfaceModel> object);
    List<SharedPtr<GraphicsInterfaceModel>> GetModels() const;

    void AddLight(SharedPtr<GraphicsInterfaceLight> light);
    void RemoveLight(SharedPtr<GraphicsInterfaceLight> light);
    List<SharedPtr<GraphicsInterfaceLight>> GetLights() const;

    void SetCamera(SharedPtr<GraphicsInterfaceCamera> camera);
    SharedPtr<GraphicsInterfaceCamera> GetCamera() const;

    void Update(F32 deltaTime);

private:
    List<SharedPtr<RenderableObject>> objects;
    List<SharedPtr<GraphicsInterfaceModel>> models;
    List<SharedPtr<GraphicsInterfaceLight>> lights;
    SharedPtr<GraphicsInterfaceCamera> camera;
};
} // namespace JzRE