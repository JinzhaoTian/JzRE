/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/Command/JzRHICommand.h"

namespace JzRE {

/**
 * @brief Draw indexed parameters
 */
struct JzDrawIndexedParams {
    JzEPrimitiveType primitiveType = JzEPrimitiveType::Triangles;
    U32              indexCount    = 0;
    U32              instanceCount = 1;
    U32              firstIndex    = 0;
    I32              vertexOffset  = 0;
    U32              firstInstance = 0;
};

/**
 * @brief Draw Indexed Command
 */
class JzRHIDrawIndexedCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param params The parameters of the command
     */
    JzRHIDrawIndexedCommand(const JzDrawIndexedParams &params);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    JzDrawIndexedParams m_params;
};

} // namespace JzRE