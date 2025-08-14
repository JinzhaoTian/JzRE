#include "JzRenderTask.h"

JzRE::JzRenderTask::JzRenderTask(JzRE::JzERenderTaskType type, JzRE::U32 priority) :
    type(type), 
    priority(priority) { }

JzRE::JzERenderTaskType JzRE::JzRenderTask::GetType() const
{
    return type;
}

JzRE::U32 JzRE::JzRenderTask::GetPriority() const
{
    return priority;
}
