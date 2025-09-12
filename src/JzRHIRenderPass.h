#pragma once

#include "JzRETypes.h"
#include "JzRHIETypes.h"

namespace JzRE {
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
    virtual void Execute(std::shared_ptr<OGLScene> scene) = 0;

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