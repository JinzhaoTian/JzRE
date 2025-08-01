#pragma once

#include "CommonTypes.h"
#include "JzRHICommandBuffer.h"
#include "JzRHIETypes.h"
#include "JzRenderTask.h"

namespace JzRE {
/**
 * @brief Command Buffer Task
 */
class JzCommandBufferTask : public JzRenderTask {
public:
    JzCommandBufferTask(std::shared_ptr<JzRHICommandBuffer> commandBuffer, U32 priority = 0);
    void Execute() override;

private:
    std::shared_ptr<JzRHICommandBuffer> commandBuffer;
};

} // namespace JzRE