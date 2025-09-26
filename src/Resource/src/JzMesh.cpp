/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Resource/JzMesh.h"

namespace JzRE {

JzMesh::JzMesh(std::vector<JzVertex> vertices, std::vector<uint32_t> indices) :
    m_vertices(std::move(vertices)), m_indices(std::move(indices))
{
    m_state = JzEResourceState::Unloaded;
}

JzMesh::~JzMesh()
{
    Unload();
}

Bool JzMesh::Load()
{
    if (m_state == JzEResourceState::Loaded) {
        return true;
    }

    m_state = JzEResourceState::Loading;

    SetupMesh();

    // If setup fails, vertex array will be null
    if (m_vertexArray) {
        m_state = JzEResourceState::Loaded;
        return true;
    } else {
        m_state = JzEResourceState::Error;
        return false;
    }
}

void JzMesh::Unload()
{
    // RHI resources will be automatically cleaned up by shared_ptr
    m_vertexBuffer = nullptr;
    m_indexBuffer  = nullptr;
    m_vertexArray  = nullptr;

    // Clear CPU data to free memory
    m_vertices.clear();
    m_vertices.shrink_to_fit();
    m_indices.clear();
    m_indices.shrink_to_fit();

    m_state = JzEResourceState::Unloaded;
}

void JzMesh::SetupMesh()
{
}

} // namespace JzRE
