/**
 * @author Gemini
 * @copyright Copyright (c) 2023-2024, Gemini
 */
#pragma once

#include "JzCommandBuffer.h"
#include <condition_variable>
#include <mutex>

namespace JzRE {

/**
 * @brief The frontend of the renderer, running on the main/logic thread.
 *
 * @details This class is the main thread's interface to the rendering system.
 * It collects render commands into a buffer and submits them to the backend
 * running on the render thread.
 */
class JzRenderFrontend {
public:
  JzRenderFrontend();

  /**
   * @brief Waits for the render thread to be ready for a new frame, then
   * prepares a new command buffer.
   */
  void BeginFrame();

  /**
   * @brief Submits a render command to the current frame's command buffer.
   *
   * @param command The command to submit.
   */
  void Submit(const JzRenderCommand &command);

  /**
   * @brief Submits the current command buffer to the render thread for
   * processing.
   */
  void Present();

private:
  friend class JzRenderBackend;

  // Called by the backend to get the command buffer to render.
  JzCommandBuffer *SwapAndGetRenderBuffer();

  // The two command buffers used for double buffering.
  JzCommandBuffer m_commandBuffers[2];
  uint8_t m_writingBufferIndex = 0;

  std::mutex m_mutex;
  std::condition_variable m_condVar;

  // Signalling flags
  bool m_isFrameReady = false;      // Is a frame ready for the RT to consume?
  bool m_isFrameFinished = true; // Has the RT finished the last frame?
};

} // namespace JzRE
