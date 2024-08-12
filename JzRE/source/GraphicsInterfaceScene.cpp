#include "GraphicsInterfaceScene.h"

namespace JzRE {
GraphicsInterfaceScene::GraphicsInterfaceScene() {
}

GraphicsInterfaceScene::~GraphicsInterfaceScene() {
    this->objects.clear();
    this->models.clear();
    this->lights.clear();
}

void GraphicsInterfaceScene::AddObject(SharedPtr<RenderableObject> object) {
    this->objects.push_back(object);
}

void GraphicsInterfaceScene::RemoveObject(SharedPtr<RenderableObject> object) {
    this->objects.erase(std::remove(objects.begin(), objects.end(), object), objects.end());
}

List<SharedPtr<RenderableObject>> GraphicsInterfaceScene::GetObjects() const {
    return this->objects;
}

void GraphicsInterfaceScene::AddModel(SharedPtr<GraphicsInterfaceModel> model) {
    this->models.push_back(model);
}

void GraphicsInterfaceScene::RemoveModel(SharedPtr<GraphicsInterfaceModel> model) {
    this->models.erase(std::remove(models.begin(), models.end(), model), models.end());
}

List<SharedPtr<GraphicsInterfaceModel>> GraphicsInterfaceScene::GetModels() const {
    return this->models;
}

void GraphicsInterfaceScene::AddLight(SharedPtr<GraphicsInterfaceLight> light) {
    lights.push_back(light);
}

void GraphicsInterfaceScene::RemoveLight(SharedPtr<GraphicsInterfaceLight> light) {
    lights.erase(std::remove(lights.begin(), lights.end(), light), lights.end());
}

List<SharedPtr<GraphicsInterfaceLight>> GraphicsInterfaceScene::GetLights() const {
    return this->lights;
}

void GraphicsInterfaceScene::SetCamera(SharedPtr<GraphicsInterfaceCamera> camera) {
    this->camera = camera;
}

SharedPtr<GraphicsInterfaceCamera> GraphicsInterfaceScene::GetCamera() const {
    return camera;
}

void GraphicsInterfaceScene::Update(F32 deltaTime) {
    for (auto &object : objects) {
        object->Update(deltaTime);
    }
}
} // namespace JzRE