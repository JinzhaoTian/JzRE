/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/Runtime/Platform/JzRHICommand.h"
#include "JzRE/Runtime/Platform/JzRHIPipeline.h"

namespace JzRE {

/**
 * @brief Bind Pipeline Command
 */
class JzRHIBindPipelineCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param pipeline The pipeline to bind
     */
    JzRHIBindPipelineCommand(std::shared_ptr<JzRHIPipeline> pipeline);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    std::shared_ptr<JzRHIPipeline> m_pipeline;
};

} // namespace JzRE