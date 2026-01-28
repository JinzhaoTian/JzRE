/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <functional>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/Threading/JzRenderTask.h"

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