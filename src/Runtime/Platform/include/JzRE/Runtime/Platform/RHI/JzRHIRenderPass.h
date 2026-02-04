/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUFramebufferObject.h"

namespace JzRE {

class JzDevice;

/**
 * @brief Render pass type
 */
enum class JzERenderPassType : U8 {
    BasePass,
    ShadowPass,
    PostProcess,
    UI,
    Debug
};

/**
 * @brief Interface of RHI Render Pass
 */
class JzRHIRenderPass {
public:
    /**
     * @brief Constructor
     * @param type The type of the render pass
     * @param name The name of the render pass
     */
    JzRHIRenderPass(JzERenderPassType type, const String &name) :
        passType(type), passName(name) { }

    /**
     * @brief Destructor
     */
    virtual ~JzRHIRenderPass() = default;

    /**
     * @brief Execute the render pass
     * @param scene The scene to render
     */
    // virtual void Execute(std::shared_ptr<JzScene> scene) = 0;

    /**
     * @brief Hook before pass execution.
     *
     * Default implementation is no-op.
     */
    virtual void OnBegin(JzDevice                               &device,
                         std::shared_ptr<JzGPUFramebufferObject> framebuffer)
    {
        (void)device;
        (void)framebuffer;
    }

    /**
     * @brief Hook after pass execution.
     *
     * Default implementation is no-op.
     */
    virtual void OnEnd(JzDevice &device)
    {
        (void)device;
    }

    /**
     * @brief Get the type of the render pass
     * @return The type of the render pass
     */
    JzERenderPassType GetPassType() const
    {
        return passType;
    }

    /**
     * @brief Get the name of the render pass
     * @return The name of the render pass
     */
    const String &GetPassName() const
    {
        return passName;
    }

protected:
    JzERenderPassType passType;
    String            passName;
};

} // namespace JzRE
