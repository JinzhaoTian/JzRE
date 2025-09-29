/**
 * @author Gemini
 * @copyright Copyright (c) 2023-2024, Gemini
 */
#pragma once

#include "JzRE/Core/JzCore.h"
#include <imgui.h>
#include <memory>
#include <variant>
#include <vector>

// Forward-declare matrix/vector types to avoid including full headers here.
namespace JzRE {
  class JzMat4;
  class JzVec2;
}

namespace JzRE {

// Forward declarations for RHI resources to avoid including heavy headers.
class JzRHIVertexBuffer;
class JzRHIIndexBuffer;
class JzRHIPipelineState;
class JzRHITexture2D;

/**
 * @brief A command to draw a mesh.
 *
 * @details This struct contains all the necessary information for the render
 * thread to issue a draw call for a single mesh or object.
 */
struct DrawMeshCommand {
  JzRef<JzRHIVertexBuffer> vertexBuffer;
  JzRef<JzRHIIndexBuffer> indexBuffer;
  JzRef<JzRHIPipelineState> pipelineState;
  JzMat4* transform;
};

/**
 * @brief A command to render ImGui draw data.
 *
 * @details This struct holds a deep copy of the data required to render a
 * full ImGui frame. A deep copy is necessary because ImGui's internal draw
 * data is transient and will be cleared on the next frame in the main thread.
 */
struct RenderImGuiCommand {
  std::vector<ImDrawVert> vertices;
  std::vector<ImDrawIdx> indices;
  std::vector<ImDrawCmd> commands;
  JzVec2 displayPos;
  JzVec2 displaySize;
  JzVec2 framebufferScale;
  JzRef<JzRHITexture2D> fontTexture;
};

/**
 * @brief A command to update the camera's view-projection matrix.
 *
 * @details This is an example of a state-update command that might be
 * needed by the rendering backend.
 */
struct UpdateCameraCommand {
  JzMat4* viewProjectionMatrix;
};

/**
 * @brief A variant representing any possible render command.
 *
 * @details This allows for storing different command types in a single
 * command buffer.
 */
using JzRenderCommand =
    std::variant<DrawMeshCommand, RenderImGuiCommand, UpdateCameraCommand>;

} // namespace JzRE
