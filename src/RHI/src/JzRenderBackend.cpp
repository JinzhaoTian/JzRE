/**
 * @author Gemini
 * @copyright Copyright (c) 2023-2024, Gemini
 */
#include "JzRE/RHI/JzRenderBackend.h"
#include "JzRE/Editor/JzWindow.h"
#include "JzRE/RHI/JzCommandBuffer.h"
#include "JzRE/RHI/JzRenderFrontend.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui_impl_opengl3.h>
#include <variant>


namespace JzRE {

// Helper struct for std::visit
template<class... Ts> struct Overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

class JzRenderBackend::RenderBackendImpl {
public:
    void Process(const DrawMeshCommand& cmd) {
        // This logic is conceptual and depends on the final RHI interface.
        auto& device = JzContext::GetInstance().GetDevice();

        device.BindPipeline(cmd.pipelineState);

        // TODO: Bind textures, update uniforms (e.g., transform matrix)
        // device.UpdateUniformBuffer(..., &cmd.transform, ...);

        device.BindVertexBuffer(cmd.vertexBuffer);
        device.BindIndexBuffer(cmd.indexBuffer);

        JzDrawIndexedParams drawParams{};
        drawParams.indexCount = cmd.indexBuffer->GetCount();
        device.DrawIndexed(drawParams);
    }

    void Process(const UpdateCameraCommand& cmd) {
        // TODO: Implement camera update logic
    }

    void Process(const RenderImGuiCommand& cmd) {
        // This logic is adapted from ImGui_ImplOpenGL3_RenderDrawData

        // Backup GL state
        GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
        glActiveTexture(GL_TEXTURE0);
        GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
        GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
        GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
        GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
        GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
        GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
        GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
        GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
        GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
        GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
        GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
        GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
        GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
        GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

        // Setup render state
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Setup viewport, orthographic projection matrix
        glViewport(0, 0, (GLsizei)cmd.displaySize.x, (GLsizei)cmd.displaySize.y);
        float L = cmd.displayPos.x;
        float R = cmd.displayPos.x + cmd.displaySize.x;
        float T = cmd.displayPos.y;
        float B = cmd.displayPos.y + cmd.displaySize.y;
        const float ortho_projection[4][4] = {
            { 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
            { 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
            { 0.0f,         0.0f,        -1.0f,   0.0f },
            { (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
        };

        // Assuming the ImGui_ImplOpenGL3_CreateDeviceObjects has been called, so shader and buffers exist.
        ImGui_ImplOpenGL3_Data* bd = ImGui_ImplOpenGL3_GetBackendData();
        glUseProgram(bd->ShaderHandle);
        glUniform1i(bd->AttribLocationTex, 0);
        glUniformMatrix4fv(bd->AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
        glBindVertexArray(bd->VaoHandle);

        // Upload vertex/index buffers
        glBindBuffer(GL_ARRAY_BUFFER, bd->VboHandle);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd.vertices.size() * sizeof(ImDrawVert), cmd.vertices.data(), GL_STREAM_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bd->EboHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd.indices.size() * sizeof(ImDrawIdx), cmd.indices.data(), GL_STREAM_DRAW);

        // Execute commands
        for (const auto& pcmd : cmd.commands) {
            ImVec4 clip_rect;
            clip_rect.x = (pcmd.ClipRect.x - cmd.displayPos.x) * cmd.framebufferScale.x;
            clip_rect.y = (pcmd.ClipRect.y - cmd.displayPos.y) * cmd.framebufferScale.y;
            clip_rect.z = (pcmd.ClipRect.z - cmd.displayPos.x) * cmd.framebufferScale.x;
            clip_rect.w = (pcmd.ClipRect.w - cmd.displayPos.y) * cmd.framebufferScale.y;

            if (clip_rect.x < cmd.displaySize.x && clip_rect.y < cmd.displaySize.y && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f) {
                glScissor((int)clip_rect.x, (int)(cmd.displaySize.y - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd.TextureId);
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd.ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd.IdxOffset * sizeof(ImDrawIdx)));
            }
        }

        // Restore modified GL state
        glUseProgram(last_program);
        glBindTexture(GL_TEXTURE_2D, last_texture);
        glActiveTexture(last_active_texture);
        glBindVertexArray(last_vertex_array);
        glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
        glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
        if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
        if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
        if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, last_polygon_mode[0]);
        glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
        glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
    }
};

JzRenderBackend::JzRenderBackend(JzRenderFrontend &frontend,
                                 JzWindow *window)
    : m_frontend(frontend), m_window(window), m_impl(std::make_unique<RenderBackendImpl>()) {}

JzRenderBackend::~JzRenderBackend() {
  if (m_isRunning) {
    Stop();
  }
}

void JzRenderBackend::Start() {
  m_isRunning = true;
  m_renderThread = std::thread(&JzRenderBackend::Run, this);
}

void JzRenderBackend::Stop() {
  {
    std::lock_guard<std::mutex> lock(m_frontend.m_mutex);
    m_isRunning = false;
    m_frontend.m_condVar.notify_one(); // Wake up the thread so it can exit
  }
  m_renderThread.join();
}

void JzRenderBackend::Run() {
  // Make the OpenGL context current on this render thread.
  glfwMakeContextCurrent(m_window->GetNativeWindow());
  // gladLoadGLLoader is crucial for the render thread to be able to make GL calls.
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  while (m_isRunning) {
    JzCommandBuffer *cmdBufferToRender = nullptr;

    {
      // Wait until a frame is ready from the main thread.
      std::unique_lock<std::mutex> lock(m_frontend.m_mutex);
      m_frontend.m_condVar.wait(lock, [this] {
        return m_frontend.m_isFrameReady || !m_isRunning;
      });

      if (!m_isRunning) {
        break; // Exit if Stop() was called
      }

      // A frame is ready, swap buffers and get the one to render.
      cmdBufferToRender = m_frontend.SwapAndGetRenderBuffer();
    }

    // --- RENDER LOGIC (mutex is unlocked) ---
    for (const auto& command : cmdBufferToRender->GetCommands()) {
        std::visit(Overloaded {
            [this](const DrawMeshCommand& cmd) { m_impl->Process(cmd); },
            [this](const RenderImGuiCommand& cmd) { m_impl->Process(cmd); },
            [this](const UpdateCameraCommand& cmd) { m_impl->Process(cmd); }
        }, command);
    }

    // After rendering, clear the buffer for the next frame.
    cmdBufferToRender->Clear();

    // Swap the buffers on the render thread
    glfwSwapBuffers(m_window->GetNativeWindow());

    // --- NOTIFY MAIN THREAD ---
    {
      // Signal that the frame is finished.
      std::lock_guard<std::mutex> lock(m_frontend.m_mutex);
      m_frontend.m_isFrameFinished = true;
      m_frontend.m_condVar.notify_one(); // Wake up the main thread
    }
  }
}

} // namespace JzRE
