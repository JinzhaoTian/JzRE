#pragma once

#include "CommonTypes.h"
#include "JzEvent.h"
#include "JzScene.h"

namespace JzRE {
/**
 * @brief Scene manager
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
    void UnloadScene();

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
    const JzScene &GetCurrentScene() const;

public:
    JzEvent<const String &> OnSceneLoaded;
    JzEvent<const String &> OnSceneUnloaded;
    JzEvent<const String &> OnSceneChanged;

private:
    UniquePtr<JzScene> m_currentScene = nullptr;
};
} // namespace JzRE