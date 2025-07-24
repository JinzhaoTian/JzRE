#include "JzSceneManager.h"

JzRE::JzSceneManager::JzSceneManager()
{
}

JzRE::JzSceneManager::~JzSceneManager()
{
}

void JzRE::JzSceneManager::Update()
{
}

void JzRE::JzSceneManager::LoadDefaultScene()
{
}

void JzRE::JzSceneManager::LoadScene(const String &scenePath)
{
}

void JzRE::JzSceneManager::UnloadScene()
{
}

JzRE::Bool JzRE::JzSceneManager::IsSceneLoaded() const
{
    return m_currentScene != nullptr;
}

const JzRE::JzScene &JzRE::JzSceneManager::GetCurrentScene() const
{
    return *m_currentScene;
}
