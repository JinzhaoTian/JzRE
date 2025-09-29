/**
 * @author Gemini
 * @copyright Copyright (c) 2023-2024, Gemini
 */
#include "JzRE/RHI/JzRenderFrontend.h"

namespace JzRE {

JzRenderFrontend::JzRenderFrontend() {}

void JzRenderFrontend::BeginFrame() {
  // Wait until the render thread has finished the previous frame.
  // This ensures we don't get more than one frame ahead.
  std::unique_lock<std::mutex> lock(m_mutex);
  m_condVar.wait(lock, [this] { return m_isFrameFinished; });

  // The render thread is idle, so we can start writing to a command buffer.
  // The write buffer is already clean from the last swap.
}

void JzRenderFrontend::Submit(const JzRenderCommand &command) {
  m_commandBuffers[m_writingBufferIndex].AddCommand(command);
}

void JzRenderFrontend::Present() {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_isFrameReady = true;
  m_isFrameFinished = false; // The new frame is not finished yet.
  m_condVar.notify_one(); // Notify the render thread that a frame is ready.
}

JzCommandBuffer *JzRenderFrontend::SwapAndGetRenderBuffer() {
  // This is called from the render thread.
  // The mutex is already locked by the render thread's loop.

  // Swap buffers
  m_writingBufferIndex = 1 - m_writingBufferIndex;
  uint8_t renderBufferIndex = 1 - m_writingBufferIndex;

  m_isFrameReady = false;

  // Return the buffer that now contains the commands to render.
  return &m_commandBuffers[renderBufferIndex];
}

} // namespace JzRE
