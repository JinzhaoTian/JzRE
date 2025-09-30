/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief Render Task Type
 */
enum class JzERenderTaskType : U8 {
    CommandBuffer,
    ResourceUpdate,
    GeometryProcessing,
    ShaderCompilation
};

/**
 * @brief Render Task Base Class
 */
class JzRenderTask {
public:
    /**
     * @brief Constructor
     *
     * @param type
     * @param priority
     */
    JzRenderTask(JzERenderTaskType type, U32 priority = 0);

    /**
     * @brief Destructor
     */
    virtual ~JzRenderTask() = default;

    /**
     * @brief Get the Type of the Render Task
     *
     * @return JzERenderTaskType
     */
    JzERenderTaskType GetType() const;

    /**
     * @brief Get the Priority of the Render Task
     *
     * @return U32
     */
    U32 GetPriority() const;

    /**
     * @brief Execute the Render Task
     */
    virtual void Execute() = 0;

protected:
    JzERenderTaskType type;
    U32               priority;
};
} // namespace JzRE