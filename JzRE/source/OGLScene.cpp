#include "OGLScene.h"

namespace JzRE {
OGLScene::OGLScene() {
}

OGLScene::~OGLScene() {
    this->objects.clear();
    this->models.clear();
    this->lights.clear();
}

void OGLScene::AddObject(SharedPtr<RenderableObject> object) {
    this->objects.push_back(object);
}

void OGLScene::RemoveObject(SharedPtr<RenderableObject> object) {
    this->objects.erase(std::remove(objects.begin(), objects.end(), object), objects.end());
}

List<SharedPtr<RenderableObject>> OGLScene::GetObjects() const {
    return this->objects;
}

void OGLScene::AddModel(SharedPtr<OGLModel> model) {
    this->models.push_back(model);
}

void OGLScene::RemoveModel(SharedPtr<OGLModel> model) {
    this->models.erase(std::remove(models.begin(), models.end(), model), models.end());
}

List<SharedPtr<OGLModel>> OGLScene::GetModels() const {
    return this->models;
}

void OGLScene::AddLight(SharedPtr<OGLLight> light) {
    lights.push_back(light);
}

void OGLScene::RemoveLight(SharedPtr<OGLLight> light) {
    lights.erase(std::remove(lights.begin(), lights.end(), light), lights.end());
}

List<SharedPtr<OGLLight>> OGLScene::GetLights() const {
    return this->lights;
}

void OGLScene::SetCamera(SharedPtr<OGLCamera> camera) {
    this->camera = camera;
}

SharedPtr<OGLCamera> OGLScene::GetCamera() const {
    return camera;
}

void OGLScene::Update(F32 deltaTime) {
    for (auto &object : objects) {
        object->Update(deltaTime);
    }
}
} // namespace JzRE