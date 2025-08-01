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
    JzRenderTask(JzERenderTaskType type, U32 priority = 0);
    virtual ~JzRenderTask() = default;

    JzERenderTaskType GetType() const;
    U32               GetPriority() const;
    virtual void      Execute() = 0;

protected:
    JzERenderTaskType type;
    U32               priority;
};
} // namespace JzRE