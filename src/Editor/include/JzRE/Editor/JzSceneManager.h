/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzEvent.h"
#include "JzRE/Runtime/Function/Scene/JzScene.h"

namespace JzRE {
/**
 * @brief Scene Manager of JzRE
 */
class JzSceneManager {
public:
    /**
     * @brief Construct a new JzSceneManager object
     */
    JzSceneManager();

    /**
     * @brief Destroy the JzSceneManager object
     */
    ~JzSceneManager();

    /**
     * @brief Update the scene manager
     */
    void Update();

    /**
     * @brief Load empty scene
     */
    void LoadEmptyScene();

    /**
     * @brief Load the default scene
     */
    void LoadDefaultScene();

    /**
     * @brief Create a new scene
     *
     * @param sceneName
     */
    void LoadScene(const String &scenePath);

    /**
     * @brief Unload the current scene
     *
     * @return const String&
     */
    void UnloadCurrentScene();

    /**
     * @brief Check if the scene is loaded
     *
     * @return Bool
     */
    Bool IsSceneLoaded() const;

    /**
     * @brief Get the current scene
     *
     * @return const JzScene&
     */
    JzScene *GetCurrentScene() const;

public:
    JzEvent<>               OnSceneLoaded;
    JzEvent<>               OnSceneUnloaded;
    JzEvent<const String &> OnSceneChanged;

private:
    std::unique_ptr<JzScene> m_currentScene = nullptr;
};
} // namespace JzRE