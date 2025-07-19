#include "OGLScene.h"

namespace JzRE {
OGLScene::OGLScene() {
}

OGLScene::~OGLScene() {
    this->models.clear();
    this->lights.clear();
}

void OGLScene::AddModel(std::shared_ptr<OGLModel> model) {
    this->models.push_back(model);
}

void OGLScene::RemoveModel(std::shared_ptr<OGLModel> model) {
    this->models.erase(std::remove(models.begin(), models.end(), model), models.end());
}

std::vector<std::shared_ptr<OGLModel>> OGLScene::GetModels() const {
    return this->models;
}

void OGLScene::AddLight(std::shared_ptr<OGLLight> light) {
    lights.push_back(light);
}

void OGLScene::RemoveLight(std::shared_ptr<OGLLight> light) {
    lights.erase(std::remove(lights.begin(), lights.end(), light), lights.end());
}

std::vector<std::shared_ptr<OGLLight>> OGLScene::GetLights() const {
    return this->lights;
}

void OGLScene::SetCamera(std::shared_ptr<OGLCamera> camera) {
    this->camera = camera;
}

std::shared_ptr<OGLCamera> OGLScene::GetCamera() const {
    return camera;
}

void OGLScene::Update(F32 deltaTime) {
    
}
} // namespace JzRE