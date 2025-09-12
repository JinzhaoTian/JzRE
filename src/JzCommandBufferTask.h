#pragma once

#include "JzRETypes.h"
#include "JzRHICommandList.h"
#include "JzRHIETypes.h"
#include "JzRenderTask.h"

namespace JzRE {
/**
 * @brief Command Buffer Task
 */
class JzCommandBufferTask : public JzRenderTask {
public:
    JzCommandBufferTask(std::shared_ptr<JzRHICommandList> commandList, U32 priority = 0);
    void Execute() override;

private:
    std::shared_ptr<JzRHICommandList> commandList;
};

} // namespace JzRE