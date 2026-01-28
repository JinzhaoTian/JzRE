/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/Command/JzRHICommandList.h"
#include "JzRE/Runtime/Platform/Threading/JzRenderTask.h"

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