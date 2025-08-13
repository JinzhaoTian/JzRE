#include "JzMesh.h"
#include "JzEditorActions.h"
#include "JzRHIDevice.h"
#include "JzServiceContainer.h"

JzRE::JzMesh::JzMesh(std::vector<JzRE::JzVertex> vertices, std::vector<JzRE::U32> indices, std::vector<std::shared_ptr<JzRE::JzRHITexture>> textures) :
    vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures))
{
    // Setup the mesh when we have all the required data
    SetupMesh();
}

JzRE::JzMesh::~JzMesh()
{
    // RHI resources will be automatically cleaned up by shared_ptr
}

void JzRE::JzMesh::Draw(std::shared_ptr<JzRE::JzRHIPipeline> pipeline)
{
    if (!m_isSetup) {
        SetupMesh();
    }

    auto device = JzRE_DEVICE();
    if (!device || !m_vertexArray || indices.empty()) {
        return;
    }

    // Bind textures using RHI
    for (U32 i = 0; i < this->textures.size(); i++) {
        device->BindTexture(this->textures[i], i);
    }

    // Bind vertex array and pipeline
    device->BindVertexArray(m_vertexArray);
    if (pipeline) {
        device->BindPipeline(pipeline);
    }

    // Draw indexed using RHI
    JzDrawIndexedParams drawParams{};
    drawParams.primitiveType = JzEPrimitiveType::Triangles;
    drawParams.indexCount    = static_cast<U32>(indices.size());
    drawParams.instanceCount = 1;
    drawParams.firstIndex    = 0;
    drawParams.vertexOffset  = 0;
    drawParams.firstInstance = 0;

    device->DrawIndexed(drawParams);
}

void JzRE::JzMesh::SetupMesh()
{
    if (m_isSetup) {
        return;
    }

    auto device = JzRE_DEVICE();
    if (!device) {
        return;
    }

    // Create vertex buffer
    JzBufferDesc vertexBufferDesc{};
    vertexBufferDesc.type      = JzEBufferType::Vertex;
    vertexBufferDesc.usage     = JzEBufferUsage::StaticDraw;
    vertexBufferDesc.size      = vertices.size() * sizeof(JzVertex);
    vertexBufferDesc.data      = vertices.data();
    vertexBufferDesc.debugName = "MeshVertexBuffer";

    m_vertexBuffer = device->CreateBuffer(vertexBufferDesc);
    if (!m_vertexBuffer) {
        return;
    }

    // Create index buffer
    JzBufferDesc indexBufferDesc{};
    indexBufferDesc.type      = JzEBufferType::Index;
    indexBufferDesc.usage     = JzEBufferUsage::StaticDraw;
    indexBufferDesc.size      = indices.size() * sizeof(U32);
    indexBufferDesc.data      = indices.data();
    indexBufferDesc.debugName = "MeshIndexBuffer";

    m_indexBuffer = device->CreateBuffer(indexBufferDesc);
    if (!m_indexBuffer) {
        return;
    }

    // Create vertex array object
    m_vertexArray = device->CreateVertexArray("MeshVertexArray");
    if (!m_vertexArray) {
        return;
    }

    // Bind vertex and index buffers to vertex array
    m_vertexArray->BindVertexBuffer(m_vertexBuffer, 0);
    m_vertexArray->BindIndexBuffer(m_indexBuffer);

    // Set vertex attributes
    // Position (location 0): vec3
    m_vertexArray->SetVertexAttribute(0, 3, sizeof(JzVertex), offsetof(JzVertex, Position));
    // Normal (location 1): vec3
    m_vertexArray->SetVertexAttribute(1, 3, sizeof(JzVertex), offsetof(JzVertex, Normal));
    // TexCoords (location 2): vec2
    m_vertexArray->SetVertexAttribute(2, 2, sizeof(JzVertex), offsetof(JzVertex, TexCoords));
    // Tangent (location 3): vec3
    m_vertexArray->SetVertexAttribute(3, 3, sizeof(JzVertex), offsetof(JzVertex, Tangent));
    // Bitangent (location 4): vec3
    m_vertexArray->SetVertexAttribute(4, 3, sizeof(JzVertex), offsetof(JzVertex, Bitangent));
    // BoneIDs (location 5): ivec4
    m_vertexArray->SetVertexAttribute(5, 4, sizeof(JzVertex), offsetof(JzVertex, BoneIDs));
    // Weights (location 6): vec4
    m_vertexArray->SetVertexAttribute(6, 4, sizeof(JzVertex), offsetof(JzVertex, Weights));

    m_isSetup = true;
}