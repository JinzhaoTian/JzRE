/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/RHI/JzRHIBindVertexArrayCommand.h"
#include "JzRE/RHI/JzDevice.h"
#include "JzRE/Core/JzServiceContainer.h"

JzRE::JzRHIBindVertexArrayCommand::JzRHIBindVertexArrayCommand(std::shared_ptr<JzRE::JzRHIVertexArray> vertexArray) :
    JzRHICommand(JzRHIECommandType::BindVertexArray),
    m_vertexArray(vertexArray) { }

void JzRE::JzRHIBindVertexArrayCommand::Execute()
{
    auto &device = JzServiceContainer::Get<JzDevice>();
    device.BindVertexArray(m_vertexArray);
}
