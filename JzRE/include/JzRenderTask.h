#pragma once

#include "CommonTypes.h"
#include "JzRHICommand.h"

namespace JzRE {
/**
 * 渲染任务类型
 */
enum class JzERenderTaskType : U8 {
    CommandBuffer,
    ResourceUpdate,
    GeometryProcessing,
    ShaderCompilation
};

/**
 * 渲染任务基类
 */
class JzRenderTask {
public:
    JzRenderTask(JzERenderTaskType type, U32 priority = 0) :
        type(type), priority(priority) { }
    virtual ~JzRenderTask() = default;

    JzERenderTaskType GetType() const
    {
        return type;
    }
    U32 GetPriority() const
    {
        return priority;
    }
    virtual void Execute() = 0;

protected:
    JzERenderTaskType type;
    U32               priority;
};
} // namespace JzRE