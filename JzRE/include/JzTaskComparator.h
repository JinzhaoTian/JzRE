#pragma once

#include "CommonTypes.h"
#include "JzRenderTask.h"

namespace JzRE {
/**
 * 任务比较器（用于优先队列）
 */
struct JzTaskComparator {
    Bool operator()(const std::shared_ptr<JzRenderTask> &a, const std::shared_ptr<JzRenderTask> &b) const
    {
        return a->GetPriority() < b->GetPriority();
    }
};
} // namespace JzRE