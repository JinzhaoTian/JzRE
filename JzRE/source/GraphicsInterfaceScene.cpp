#include "GraphicsInterfaceScene.h"

namespace JzRE {
GraphicsInterfaceScene::GraphicsInterfaceScene() {
}

GraphicsInterfaceScene::~GraphicsInterfaceScene() {
}

void GraphicsInterfaceScene::AddObject(SharedPtr<RenderableObject> object) {
    objects.push_back(object);
}

void GraphicsInterfaceScene::RemoveObject(SharedPtr<RenderableObject> object) {
    objects.erase(std::remove(objects.begin(), objects.end(), object), objects.end());
}

// void GraphicsInterfaceScene::AddLight(SharedPtr<Light> light) {
//     lights.push_back(light);
// }

// void GraphicsInterfaceScene::RemoveLight(SharedPtr<Light> light) {
//     lights.erase(std::remove(lights.begin(), lights.end(), light), lights.end());
// }

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

void GraphicsInterfaceScene::Draw() const {
    // for (const auto &light : lights) {
    //     light->apply(shader);
    // }

    for (const auto &object : objects) {
        if (camera) {
            object->GetShader()->SetUniform("view", camera->GetViewMatrix());
            object->GetShader()->SetUniform("projection", camera->GetProjectionMatrix());
        }
        object->Draw();
    }
}
} // namespace JzRE