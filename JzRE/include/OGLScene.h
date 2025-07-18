#pragma once

#include "CommonTypes.h"
#include "OGLModel.h"
#include "OGLShader.h"
#include "OGLCamera.h"
#include "OGLLight.h"

namespace JzRE {
class OGLScene {
public:
    OGLScene();
    ~OGLScene();

    void AddModel(SharedPtr<OGLModel> object);
    void RemoveModel(SharedPtr<OGLModel> object);
    List<SharedPtr<OGLModel>> GetModels() const;

    void AddLight(SharedPtr<OGLLight> light);
    void RemoveLight(SharedPtr<OGLLight> light);
    List<SharedPtr<OGLLight>> GetLights() const;

    void SetCamera(SharedPtr<OGLCamera> camera);
    SharedPtr<OGLCamera> GetCamera() const;

    void Update(F32 deltaTime);

private:
    List<SharedPtr<OGLModel>> models;
    List<SharedPtr<OGLLight>> lights;
    SharedPtr<OGLCamera> camera;
};
} // namespace JzRE