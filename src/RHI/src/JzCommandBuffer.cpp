/**
 * @author Gemini
 * @copyright Copyright (c) 2023-2024, Gemini
 */
#include "JzRE/RHI/JzCommandBuffer.h"

namespace JzRE {

void JzCommandBuffer::AddCommand(const JzRenderCommand &command) {
  m_commands.push_back(command);
}

void JzCommandBuffer::Clear() { m_commands.clear(); }

const std::vector<JzRenderCommand> &JzCommandBuffer::GetCommands() const {
  return m_commands;
}

} // namespace JzRE
