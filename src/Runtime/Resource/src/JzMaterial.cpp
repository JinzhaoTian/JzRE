/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Resource/JzMaterial.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Platform/RHI/JzDevice.h"

JzRE::JzMaterial::JzMaterial(const JzRE::String &path)
{
    m_state = JzEResourceState::Unloaded;
}

JzRE::JzMaterial::JzMaterial(const JzRE::JzMaterialProperties &properties) :
    m_properties(properties)
{
    m_state = JzEResourceState::Loaded; // Material properties are already set
}

JzRE::Bool JzRE::JzMaterial::Load()
{
    if (m_state == JzEResourceState::Loaded) {
        return true;
    }
    m_state = JzEResourceState::Loading;

    // In a real engine, you would load material properties from a file,
    // then load the specified shader, and create a pipeline based on it.
    // Here, we'll just create a placeholder pipeline.

    auto &device = JzServiceContainer::Get<JzDevice>();

    JzPipelineDesc pipelineDesc;
    // pipelineDesc.shader = ... load shader from resource manager ...
    // ... configure other pipeline states ...

    m_pipeline = device.CreatePipeline(pipelineDesc);

    if (m_pipeline) {
        m_state = JzEResourceState::Loaded;
        return true;
    } else {
        m_state = JzEResourceState::Error;
        return false;
    }
}

void JzRE::JzMaterial::Unload()
{
    m_pipeline = nullptr;
    m_textures.clear();
    m_textures.shrink_to_fit();
    m_state = JzEResourceState::Unloaded;
}
