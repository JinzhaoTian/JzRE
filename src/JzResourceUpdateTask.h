#pragma once

#include <functional>
#include "JzRETypes.h"
#include "JzRenderTask.h"

namespace JzRE {
/**
 * @brief Resource Update Task
 */
class JzResourceUpdateTask : public JzRenderTask {
public:
    JzResourceUpdateTask(std::function<void()> updateFunc, U32 priority = 1);
    void Execute() override;

private:
    std::function<void()> updateFunc;
};

} // namespace JzRE