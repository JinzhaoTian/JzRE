/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Platform/Command/JzRHICommand.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"

namespace JzRE {

/**
 * @brief Bind Texture Command
 */
class JzRHIBindTextureCommand : public JzRHICommand {
public:
    /**
     * @brief Constructor
     * @param texture The texture to bind
     * @param slot The slot to bind the texture to
     */
    JzRHIBindTextureCommand(std::shared_ptr<JzGPUTextureObject> texture, U32 slot);

    /**
     * @brief Execute the command
     */
    void Execute() override;

private:
    std::shared_ptr<JzGPUTextureObject> m_texture;
    U32                                 m_slot;
};

} // namespace JzRE