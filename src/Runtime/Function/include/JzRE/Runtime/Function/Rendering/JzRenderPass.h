/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <functional>
#include <memory>

#include "JzRE/Runtime/Core/JzMatrix.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/Rendering/JzRenderTarget.h"
#include "JzRE/Runtime/Platform/Command/JzRHIDrawCommand.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUVertexArrayObject.h"
#include "JzRE/Runtime/Platform/RHI/JzRHIPipeline.h"

namespace JzRE {

/**
 * @brief Description for a feature-gated render pass.
 *
 * @deprecated Prefer JzRenderGraphContribution registration in JzRenderSystem.
 *
 * Render passes are executed after filtered entity rendering in each target.
 * Each pass is controlled by a feature flag (Skybox/Grid/Axis/...) and
 * shares a unified execution path in RenderSystem as a compatibility adapter.
 */
struct JzRenderPass {
    String                                  name;
    JzRenderTargetFeatures                  feature = JzRenderTargetFeatures::None;
    std::shared_ptr<JzRHIPipeline>          pipeline;
    std::shared_ptr<JzGPUVertexArrayObject> vertexArray;
    JzDrawParams                            drawParams;
    std::function<void(const std::shared_ptr<JzRHIPipeline> &, JzWorld &,
                       const JzMat4 &, const JzMat4 &)>
        setupPass;
};

} // namespace JzRE
