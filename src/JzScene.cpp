/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzScene.h"

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
    // create cube vertices
    std::vector<JzVertex> vertices;

    // cube 8 vertices
    JzVec3 positions[8] = {
        // front
        JzVec3(-0.5f, -0.5f, 0.5f), // left bottom front
        JzVec3(0.5f, -0.5f, 0.5f),  // right bottom front
        JzVec3(0.5f, 0.5f, 0.5f),   // right top front
        JzVec3(-0.5f, 0.5f, 0.5f),  // left top front

        // back
        JzVec3(-0.5f, -0.5f, -0.5f), // left bottom back
        JzVec3(0.5f, -0.5f, -0.5f),  // right bottom back
        JzVec3(0.5f, 0.5f, -0.5f),   // right top back
        JzVec3(-0.5f, 0.5f, -0.5f)   // left top back
    };

    // cube needs 24 vertices (4 vertices per face)
    // front (Z+)
    vertices.push_back({positions[0], JzVec3(0.0f, 0.0f, 1.0f), JzVec2(0.0f, 0.0f), JzVec3(1.0f, 0.0f, 0.0f), JzVec3(0.0f, 1.0f, 0.0f), {0}, {0.0f}});
    vertices.push_back({positions[1], JzVec3(0.0f, 0.0f, 1.0f), JzVec2(1.0f, 0.0f), JzVec3(1.0f, 0.0f, 0.0f), JzVec3(0.0f, 1.0f, 0.0f), {0}, {0.0f}});
    vertices.push_back({positions[2], JzVec3(0.0f, 0.0f, 1.0f), JzVec2(1.0f, 1.0f), JzVec3(1.0f, 0.0f, 0.0f), JzVec3(0.0f, 1.0f, 0.0f), {0}, {0.0f}});
    vertices.push_back({positions[3], JzVec3(0.0f, 0.0f, 1.0f), JzVec2(0.0f, 1.0f), JzVec3(1.0f, 0.0f, 0.0f), JzVec3(0.0f, 1.0f, 0.0f), {0}, {0.0f}});

    // back (Z-)
    vertices.push_back({positions[5], JzVec3(0.0f, 0.0f, -1.0f), JzVec2(0.0f, 0.0f), JzVec3(-1.0f, 0.0f, 0.0f), JzVec3(0.0f, 1.0f, 0.0f), {0}, {0.0f}});
    vertices.push_back({positions[4], JzVec3(0.0f, 0.0f, -1.0f), JzVec2(1.0f, 0.0f), JzVec3(-1.0f, 0.0f, 0.0f), JzVec3(0.0f, 1.0f, 0.0f), {0}, {0.0f}});
    vertices.push_back({positions[7], JzVec3(0.0f, 0.0f, -1.0f), JzVec2(1.0f, 1.0f), JzVec3(-1.0f, 0.0f, 0.0f), JzVec3(0.0f, 1.0f, 0.0f), {0}, {0.0f}});
    vertices.push_back({positions[6], JzVec3(0.0f, 0.0f, -1.0f), JzVec2(0.0f, 1.0f), JzVec3(-1.0f, 0.0f, 0.0f), JzVec3(0.0f, 1.0f, 0.0f), {0}, {0.0f}});

    // right (X+)
    vertices.push_back({positions[1], JzVec3(1.0f, 0.0f, 0.0f), JzVec2(0.0f, 0.0f), JzVec3(0.0f, 0.0f, -1.0f), JzVec3(0.0f, 1.0f, 0.0f), {0}, {0.0f}});
    vertices.push_back({positions[5], JzVec3(1.0f, 0.0f, 0.0f), JzVec2(1.0f, 0.0f), JzVec3(0.0f, 0.0f, -1.0f), JzVec3(0.0f, 1.0f, 0.0f), {0}, {0.0f}});
    vertices.push_back({positions[6], JzVec3(1.0f, 0.0f, 0.0f), JzVec2(1.0f, 1.0f), JzVec3(0.0f, 0.0f, -1.0f), JzVec3(0.0f, 1.0f, 0.0f), {0}, {0.0f}});
    vertices.push_back({positions[2], JzVec3(1.0f, 0.0f, 0.0f), JzVec2(0.0f, 1.0f), JzVec3(0.0f, 0.0f, -1.0f), JzVec3(0.0f, 1.0f, 0.0f), {0}, {0.0f}});

    // left (X-)
    vertices.push_back({positions[4], JzVec3(-1.0f, 0.0f, 0.0f), JzVec2(0.0f, 0.0f), JzVec3(0.0f, 0.0f, 1.0f), JzVec3(0.0f, 1.0f, 0.0f), {0}, {0.0f}});
    vertices.push_back({positions[0], JzVec3(-1.0f, 0.0f, 0.0f), JzVec2(1.0f, 0.0f), JzVec3(0.0f, 0.0f, 1.0f), JzVec3(0.0f, 1.0f, 0.0f), {0}, {0.0f}});
    vertices.push_back({positions[3], JzVec3(-1.0f, 0.0f, 0.0f), JzVec2(1.0f, 1.0f), JzVec3(0.0f, 0.0f, 1.0f), JzVec3(0.0f, 1.0f, 0.0f), {0}, {0.0f}});
    vertices.push_back({positions[7], JzVec3(-1.0f, 0.0f, 0.0f), JzVec2(0.0f, 1.0f), JzVec3(0.0f, 0.0f, 1.0f), JzVec3(0.0f, 1.0f, 0.0f), {0}, {0.0f}});

    // top (Y+)
    vertices.push_back({positions[3], JzVec3(0.0f, 1.0f, 0.0f), JzVec2(0.0f, 0.0f), JzVec3(1.0f, 0.0f, 0.0f), JzVec3(0.0f, 0.0f, 1.0f), {0}, {0.0f}});
    vertices.push_back({positions[2], JzVec3(0.0f, 1.0f, 0.0f), JzVec2(1.0f, 0.0f), JzVec3(1.0f, 0.0f, 0.0f), JzVec3(0.0f, 0.0f, 1.0f), {0}, {0.0f}});
    vertices.push_back({positions[6], JzVec3(0.0f, 1.0f, 0.0f), JzVec2(1.0f, 1.0f), JzVec3(1.0f, 0.0f, 0.0f), JzVec3(0.0f, 0.0f, 1.0f), {0}, {0.0f}});
    vertices.push_back({positions[7], JzVec3(0.0f, 1.0f, 0.0f), JzVec2(0.0f, 1.0f), JzVec3(1.0f, 0.0f, 0.0f), JzVec3(0.0f, 0.0f, 1.0f), {0}, {0.0f}});

    // bottom (Y-)
    vertices.push_back({positions[0], JzVec3(0.0f, -1.0f, 0.0f), JzVec2(0.0f, 0.0f), JzVec3(1.0f, 0.0f, 0.0f), JzVec3(0.0f, 0.0f, -1.0f), {0}, {0.0f}});
    vertices.push_back({positions[4], JzVec3(0.0f, -1.0f, 0.0f), JzVec2(1.0f, 0.0f), JzVec3(1.0f, 0.0f, 0.0f), JzVec3(0.0f, 0.0f, -1.0f), {0}, {0.0f}});
    vertices.push_back({positions[5], JzVec3(0.0f, -1.0f, 0.0f), JzVec2(1.0f, 1.0f), JzVec3(1.0f, 0.0f, 0.0f), JzVec3(0.0f, 0.0f, -1.0f), {0}, {0.0f}});
    vertices.push_back({positions[1], JzVec3(0.0f, -1.0f, 0.0f), JzVec2(0.0f, 1.0f), JzVec3(1.0f, 0.0f, 0.0f), JzVec3(0.0f, 0.0f, -1.0f), {0}, {0.0f}});

    // create index data (2 triangles per face, 12 triangles in total)
    std::vector<U32> indices = {
        // front
        0, 1, 2, 2, 3, 0,
        // back
        4, 5, 6, 6, 7, 4,
        // right
        8, 9, 10, 10, 11, 8,
        // left
        16, 17, 18, 18, 19, 16,
        // bottom
        20, 21, 22, 22, 23, 20};

    // create empty texture array (default cube doesn't need textures)
    std::vector<std::shared_ptr<JzRHITexture>> textures;

    // create mesh
    JzMesh cubeMesh(vertices, indices, textures);

    // create model
    std::vector<JzMesh> meshes;
    meshes.push_back(std::move(cubeMesh));
    auto cubeModel = std::make_shared<JzModel>(std::move(meshes));

    // add model to scene
    AddModel(cubeModel);
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
