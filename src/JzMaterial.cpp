/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzMaterial.h"
#include "JzContext.h"
#include "JzRHIDevice.h"
#include "JzRHIDescription.h"

namespace JzRE {

Bool JzMaterial::Load() {
    if (m_state == JzEResourceState::Loaded) {
        return true;
    }
    m_state = JzEResourceState::Loading;

    // In a real engine, you would load material properties from a file,
    // then load the specified shader, and create a pipeline based on it.
    // Here, we'll just create a placeholder pipeline.

    auto& device = JzRE_DEVICE();

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

void JzMaterial::Unload() {
    m_pipeline = nullptr;
    m_textures.clear();
    m_textures.shrink_to_fit();
    m_state = JzEResourceState::Unloaded;
}

} // namespace JzRE
