#pragma once

#include "CommonTypes.h"
#include "JzRHITypes.h"

namespace JzRE {
/**
 * 命令缓冲任务
 */
class JzCommandBufferTask : public JzRenderTask {
public:
    JzCommandBufferTask(std::shared_ptr<JzRHICommandBuffer> commandBuffer, U32 priority = 0);
    void JzExecute() override;

private:
    std::shared_ptr<JzRHICommandBuffer> commandBuffer;
};

} // namespace JzRE