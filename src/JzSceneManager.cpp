/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzSceneManager.h"

JzRE::JzSceneManager::JzSceneManager()
{
    LoadEmptyScene();
}

JzRE::JzSceneManager::~JzSceneManager()
{
    UnloadCurrentScene();
}

void JzRE::JzSceneManager::Update()
{
}

void JzRE::JzSceneManager::LoadEmptyScene()
{
    UnloadCurrentScene();

    m_currentScene.reset(new JzScene());

    OnSceneLoaded.Invoke();
}

void JzRE::JzSceneManager::LoadDefaultScene()
{
    m_currentScene.reset(new JzScene());
    m_currentScene->AddDefaultCamera();
    m_currentScene->AddDefaultLights();
    m_currentScene->AddDefaultReflections();
    m_currentScene->AddDefaultPostProcessStack();
    m_currentScene->AddDefaultAtmosphere();

    m_currentScene->AddDefaultModel(); // for test

    OnSceneLoaded.Invoke();
}

void JzRE::JzSceneManager::LoadScene(const String &scenePath)
{
    auto model = std::make_shared<JzModel>(scenePath, false);
    m_currentScene->AddModel(model);
}

void JzRE::JzSceneManager::UnloadCurrentScene()
{
    if (m_currentScene) {
        m_currentScene.reset();
        OnSceneUnloaded.Invoke();
    }
}

JzRE::Bool JzRE::JzSceneManager::IsSceneLoaded() const
{
    return m_currentScene != nullptr;
}

JzRE::JzScene *JzRE::JzSceneManager::GetCurrentScene() const
{
    return m_currentScene.get();
}
