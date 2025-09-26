/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzScene.h"
#include "JzRE/Resource/JzModel.h"

JzRE::JzScene::JzScene() { }

JzRE::JzScene::~JzScene()
{
    this->m_models.clear();
    this->m_lights.clear();
}

void JzRE::JzScene::AddDefaultCamera() { }

void JzRE::JzScene::AddDefaultLights() { }

void JzRE::JzScene::AddDefaultReflections() { }

void JzRE::JzScene::AddDefaultPostProcessStack() { }

void JzRE::JzScene::AddDefaultSkysphere() { }

void JzRE::JzScene::AddDefaultAtmosphere() { }

void JzRE::JzScene::AddDefaultModel()
{
}

void JzRE::JzScene::AddModel(std::shared_ptr<JzRE::JzModel> model)
{
    this->m_models.push_back(model);
}

void JzRE::JzScene::RemoveModel(std::shared_ptr<JzRE::JzModel> model)
{
    this->m_models.erase(std::remove(m_models.begin(), m_models.end(), model), m_models.end());
}

std::vector<std::shared_ptr<JzRE::JzModel>> JzRE::JzScene::GetModels() const
{
    return this->m_models;
}

void JzRE::JzScene::AddLight(std::shared_ptr<JzRE::JzLight> light)
{
    m_lights.push_back(light);
}

void JzRE::JzScene::RemoveLight(std::shared_ptr<JzRE::JzLight> light)
{
    m_lights.erase(std::remove(m_lights.begin(), m_lights.end(), light), m_lights.end());
}

std::vector<std::shared_ptr<JzRE::JzLight>> JzRE::JzScene::GetLights() const
{
    return this->m_lights;
}

void JzRE::JzScene::Update(JzRE::F32 deltaTime) { }

JzRE::JzCamera *JzRE::JzScene::FindMainCamera() const
{
    return m_camera.get();
}

void JzRE::JzScene::SetCamera(std::shared_ptr<JzCamera> camera)
{
    m_camera = camera;
}

void JzRE::JzScene::Serialize(const String &filePath) { }

void JzRE::JzScene::Deserialize(const String &filePath) { }
