#pragma once

#include "CommonTypes.h"
#include "JzModel.h"
#include "OGLCamera.h"
#include "OGLLight.h"
#include "OGLShader.h"

namespace JzRE {
class OGLScene {
public:
    OGLScene();
    ~OGLScene();

    void                                  AddModel(std::shared_ptr<JzModel> object);
    void                                  RemoveModel(std::shared_ptr<JzModel> object);
    std::vector<std::shared_ptr<JzModel>> GetModels() const;

    void                                   AddLight(std::shared_ptr<OGLLight> light);
    void                                   RemoveLight(std::shared_ptr<OGLLight> light);
    std::vector<std::shared_ptr<OGLLight>> GetLights() const;

    void                       SetCamera(std::shared_ptr<OGLCamera> camera);
    std::shared_ptr<OGLCamera> GetCamera() const;

    void Update(F32 deltaTime);

private:
    std::vector<std::shared_ptr<JzModel>>  models;
    std::vector<std::shared_ptr<OGLLight>> lights;
    std::shared_ptr<OGLCamera>             camera;
};
} // namespace JzRE