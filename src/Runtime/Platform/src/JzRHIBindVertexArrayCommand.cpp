/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Platform/JzRHIBindVertexArrayCommand.h"
#include "JzRE/Runtime/Platform/JzDevice.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"

JzRE::JzRHIBindVertexArrayCommand::JzRHIBindVertexArrayCommand(std::shared_ptr<JzRE::JzGPUVertexArrayObject> vertexArray) :
    JzRHICommand(JzRHIECommandType::BindVertexArray),
    m_vertexArray(vertexArray) { }

void JzRE::JzRHIBindVertexArrayCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.BindVertexArray(m_vertexArray);
}
