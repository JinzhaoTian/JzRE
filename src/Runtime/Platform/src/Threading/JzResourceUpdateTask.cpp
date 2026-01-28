/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Platform/Threading/JzResourceUpdateTask.h"

JzRE::JzResourceUpdateTask::JzResourceUpdateTask(std::function<void()> updateFunc, U32 priority) :
    JzRE::JzRenderTask(JzRE::JzERenderTaskType::ResourceUpdate, priority) { }

void JzRE::JzResourceUpdateTask::Execute() { }