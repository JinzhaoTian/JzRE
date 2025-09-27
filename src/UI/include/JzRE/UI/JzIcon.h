/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzVector.h"
#include "JzRE/RHI/JzRHITexture.h"
#include "JzRE/UI/JzWidget.h"

namespace JzRE {
/**
 * @brief Image Widget
 */
class JzIcon : public JzWidget {
public:
    /**
     * @brief Constructor
     *
     * @param texture
     * @param size
     */
    JzIcon(std::shared_ptr<JzRHITexture> texture, const JzVec2 &size = {16.f, 16.f});

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

private:
    JzVec2                        m_textureSize;
    std::shared_ptr<JzRHITexture> m_texture;
};
} // namespace JzRE