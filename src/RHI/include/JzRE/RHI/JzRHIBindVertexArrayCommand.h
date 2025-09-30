/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/RHI/JzRHICommand.h"
#include "JzRE/RHI/JzRHIVertexArray.h"

namespace JzRE {

/**
 * @brief Bind Vertex Array Command
 */
class JzRHIBindVertexArrayCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param vertexArray The vertex array to bind
     */
    JzRHIBindVertexArrayCommand(std::shared_ptr<JzRHIVertexArray> vertexArray);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    std::shared_ptr<JzRHIVertexArray> m_vertexArray;
};

} // namespace JzRE