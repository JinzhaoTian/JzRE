/**
 * @author Gemini
 * @copyright Copyright (c) 2023-2024, Gemini
 */
#pragma once

#include "JzRenderCommand.h"
#include <vector>

namespace JzRE {

/**
 * @brief A container for a sequence of render commands for a single frame.
 */
class JzCommandBuffer {
public:
  /**
   * @brief Adds a new render command to the end of the buffer.
   *
   * @param command The render command to add.
   */
  void AddCommand(const JzRenderCommand &command);

  /**
   * @brief Clears all commands from the buffer.
   */
  void Clear();

  /**
   * @brief Gets a constant reference to the list of commands.
   *
   * @return A const reference to the internal vector of commands.
   */
  const std::vector<JzRenderCommand> &GetCommands() const;

private:
  std::vector<JzRenderCommand> m_commands;
};

} // namespace JzRE
