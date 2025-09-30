/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <imgui.h>
#include <memory>
#include <vector>
#include "JzRE/Core/JzVector.h"
#include "JzRE/RHI/JzRHICommand.h"
#include "JzRE/RHI/JzRHITexture.h"

namespace JzRE {

/**
 * @brief ImGui Render Parameters
 */
struct JzImGuiRenderParams {
    std::vector<ImDrawVert>       vertices;
    std::vector<ImDrawIdx>        indices;
    std::vector<ImDrawCmd>        commands;
    JzVec2                        displayPos;
    JzVec2                        displaySize;
    JzVec2                        framebufferScale;
    std::shared_ptr<JzRHITexture> fontTexture;
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