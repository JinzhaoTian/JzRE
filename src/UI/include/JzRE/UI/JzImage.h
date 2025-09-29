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
class JzImage : public JzWidget {
public:
    /**
     * Constructor
     *
     * @param p_textureId
     * @param p_size
     */
    JzImage(std::shared_ptr<JzRHITexture> imageTexture, const JzVec2 &imageSize);

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

public:
    std::shared_ptr<JzRHITexture> imageTexture;
    JzVec2                        imageSize;
};

} // namespace JzRE