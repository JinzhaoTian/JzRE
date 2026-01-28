/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <imgui.h>
#include <memory>
#include <vector>
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Platform/Command/JzRHICommand.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"

namespace JzRE {

/**
 * @brief ImGui Render Parameters
 */
struct JzImGuiRenderParams {
    std::vector<ImDrawVert>             vertices;
    std::vector<ImDrawIdx>              indices;
    std::vector<ImDrawCmd>              commands;
    JzVec2                              displayPos;
    JzVec2                              displaySize;
    JzVec2                              framebufferScale;
    std::shared_ptr<JzGPUTextureObject> fontTexture;
};

/**
 * @brief ImGui Render Command
 */
class JzRHIImGuiRenderCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     *
     * @param params
     */
    JzRHIImGuiRenderCommand(const JzImGuiRenderParams &params);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    JzImGuiRenderParams m_params;
};

} // namespace JzRE