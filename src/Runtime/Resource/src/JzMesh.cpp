/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Resource/JzMesh.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Platform/JzDevice.h"

namespace JzRE {

JzMesh::JzMesh(const String &path)
{
    m_name  = path;
    m_state = JzEResourceState::Unloaded;
}

JzMesh::JzMesh(std::vector<JzVertex> vertices, std::vector<uint32_t> indices, I32 materialIndex) :
    m_vertices(std::move(vertices)), m_indices(std::move(indices)), m_materialIndex(materialIndex)
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
    auto &device = JzServiceContainer::Get<JzDevice>();

    // Create vertex buffer
    JzGPUBufferObjectDesc vbDesc;
    vbDesc.type      = JzEGPUBufferObjectType::Vertex;
    vbDesc.usage     = JzEGPUBufferObjectUsage::StaticDraw;
    vbDesc.size      = m_vertices.size() * sizeof(JzVertex);
    vbDesc.data      = m_vertices.data();
    vbDesc.debugName = "MeshVB";
    m_vertexBuffer   = device.CreateBuffer(vbDesc);

    if (!m_vertexBuffer) {
        return;
    }

    // Create index buffer
    JzGPUBufferObjectDesc ibDesc;
    ibDesc.type      = JzEGPUBufferObjectType::Index;
    ibDesc.usage     = JzEGPUBufferObjectUsage::StaticDraw;
    ibDesc.size      = m_indices.size() * sizeof(U32);
    ibDesc.data      = m_indices.data();
    ibDesc.debugName = "MeshIB";
    m_indexBuffer    = device.CreateBuffer(ibDesc);

    if (!m_indexBuffer) {
        return;
    }

    // Create vertex array object
    m_vertexArray = device.CreateVertexArray("MeshVAO");
    if (!m_vertexArray) {
        return;
    }

    // Bind buffers to VAO
    m_vertexArray->BindVertexBuffer(m_vertexBuffer, 0);
    m_vertexArray->BindIndexBuffer(m_indexBuffer);

    // Set vertex attributes (matching JzVertex struct layout)
    // Position (location 0): 3 floats at offset 0
    m_vertexArray->SetVertexAttribute(0, 3, sizeof(JzVertex), offsetof(JzVertex, Position));
    // Normal (location 1): 3 floats
    m_vertexArray->SetVertexAttribute(1, 3, sizeof(JzVertex), offsetof(JzVertex, Normal));
    // TexCoords (location 2): 2 floats
    m_vertexArray->SetVertexAttribute(2, 2, sizeof(JzVertex), offsetof(JzVertex, TexCoords));
    // Tangent (location 3): 3 floats
    m_vertexArray->SetVertexAttribute(3, 3, sizeof(JzVertex), offsetof(JzVertex, Tangent));
    // Bitangent (location 4): 3 floats
    m_vertexArray->SetVertexAttribute(4, 3, sizeof(JzVertex), offsetof(JzVertex, Bitangent));
}

} // namespace JzRE
