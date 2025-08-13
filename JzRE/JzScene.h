#pragma once

#include "CommonTypes.h"
#include "JzCamera.h"
#include "JzISerializable.h"
#include "JzLight.h"
#include "JzModel.h"

namespace JzRE {
/**
 * @brief Scene
 */
class JzScene : public JzISerializable {
public:
    /**
     * @brief Construct a new JzScene object
     */
    JzScene();

    /**
     * @brief Destroy the JzScene object
     */
    ~JzScene();

    /**
     * @brief Add a default camera to the scene
     */
    void AddDefaultCamera();

    /**
     * @brief Add default lights to the scene
     */
    void AddDefaultLights();

    /**
     * @brief Add default lights to the scene
     */
    void AddDefaultReflections();

    /**
     * @brief Add default post-process stack to the scene
     */
    void AddDefaultPostProcessStack();

    /**
     * @brief Add default skysphere to the scene
     */
    void AddDefaultSkysphere();

    /**
     * @brief Add default atmosphere to the scene
     */
    void AddDefaultAtmosphere();

    /**
     * @brief Add a default smodel to the scene
     */
    void AddDefaultModel();

    /**
     * @brief Add a model to the scene
     *
     * @param model The model to add
     */
    void AddModel(std::shared_ptr<JzModel> model);

    /**
     * @brief Remove a model from the scene
     *
     *  @param object The model to remove
     */
    void RemoveModel(std::shared_ptr<JzModel> object);

    /**
     * @brief Get the models in the scene
     *
     * @return The models in the scene
     */
    std::vector<std::shared_ptr<JzModel>> GetModels() const;

    /**
     * @brief Add a light to the scene
     *
     * @param light The light to add
     */
    void AddLight(std::shared_ptr<JzLight> light);

    /**
     * @brief Remove a light from the scene
     *
     *  @param light The light to remove
     */
    void RemoveLight(std::shared_ptr<JzLight> light);

    /**
     * @brief Get the lights in the scene
     *
     * @return The lights in the scene
     */
    std::vector<std::shared_ptr<JzLight>> GetLights() const;

    /**
     * @brief Update
     *
     * @param deltaTime
     */
    void Update(F32 deltaTime);

    /**
     * @brief Find the main camera in the scene
     *
     * @return The main camera
     */
    JzCamera *FindMainCamera() const;

    /**
     * @brief Set the camera in the scene
     *
     *  @param camera The camera to set
     */
    void SetCamera(std::shared_ptr<JzCamera> camera);

    /**
     * @brief Serialize the scene
     *
     * @param filePath
     */
    virtual void Serialize(const String &filePath) override;

    /**
     * @brief Deserialize the scene
     *
     * @param filePath
     */
    virtual void Deserialize(const String &filePath) override;

private:
    std::vector<std::shared_ptr<JzModel>> m_models;
    std::vector<std::shared_ptr<JzLight>> m_lights;
    std::shared_ptr<JzCamera>             m_camera;
};
} // namespace JzRE