/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzVector.h"
#include "JzRE/UI/JzWidget.h"

namespace JzRE {

/**
 * @brief Frame widget for displaying textures
 */
class JzFrame : public JzWidget {
public:
    /**
     * @brief Constructor
     */
    JzFrame();

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

public:
    void  *frameTextureId;
    JzVec2 frameSize;
};

} // namespace JzRE