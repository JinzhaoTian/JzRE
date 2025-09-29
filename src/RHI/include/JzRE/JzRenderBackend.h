/**
 * @author Gemini
 * @copyright Copyright (c) 2023-2024, Gemini
 */
#pragma once

#include <thread>

namespace JzRE {

class JzRenderFrontend; // Forward declaration

/**
 * @brief The backend of the renderer, running on its own dedicated thread.
 *
 * @details This class waits for command buffers from the frontend, processes
 * them by making RHI calls, and manages the render thread's lifecycle.
 */
class JzRenderBackend {
public:
  JzRenderBackend(JzRenderFrontend &frontend);
  ~JzRenderBackend();

  /**
   * @brief Starts the render thread.
   */
  void Start();

  /**
   * @brief Stops the render thread and waits for it to join.
   */
  void Stop();

private:
  /**
   * @brief The main function for the render thread.
   */
  void Run();

  JzRenderFrontend &m_frontend;
  std::thread m_renderThread;
  bool m_isRunning = false;
};

} // namespace JzRE
