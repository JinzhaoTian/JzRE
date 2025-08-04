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

    void AddModel(std::shared_ptr<JzModel> object);

    void RemoveModel(std::shared_ptr<JzModel> object);

    std::vector<std::shared_ptr<JzModel>> GetModels() const;

    void AddLight(std::shared_ptr<JzLight> light);

    void RemoveLight(std::shared_ptr<JzLight> light);

    std::vector<std::shared_ptr<JzLight>> GetLights() const;

    /**
     * @brief Update
     *
     * @param deltaTime
     */
    void Update(F32 deltaTime);

    JzCamera *FindMainCamera() const;

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