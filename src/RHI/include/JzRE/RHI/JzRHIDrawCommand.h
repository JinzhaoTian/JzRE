/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/RHI/JzRHICommand.h"

namespace JzRE {

/**
 * @brief Draw parameters
 */
struct JzDrawParams {
    JzEPrimitiveType primitiveType = JzEPrimitiveType::Triangles;
    U32              vertexCount   = 0;
    U32              instanceCount = 1;
    U32              firstVertex   = 0;
    U32              firstInstance = 0;
};

/**
 * @brief Draw Command
 */
class JzRHIDrawCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param params The parameters of the command
     */
    JzRHIDrawCommand(const JzDrawParams &params);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    JzDrawParams m_params;
};

} // namespace JzRE