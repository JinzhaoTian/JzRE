/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRETypes.h"
#include "JzEvent.h"
#include "JzWidget.h"
#include "JzVector.h"
#include "JzRHITexture.h"

namespace JzRE {

/**
 * @brief Image Button Widget
 */
class JzImageButton : public JzWidget {
public:
    /**
     * @brief Constructor
     *
     * @param p_texture
     * @param p_size
     */
    JzImageButton(std::shared_ptr<JzRHITexture> p_texture, const JzVec2 &p_size);

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

public:
    /**
     * @brief Event triggered when the button is clicked
     */
    JzEvent<> ClickedEvent;

public:
    std::shared_ptr<JzRHITexture> m_texture;
    Bool   disabled = false;
    JzVec2 size;
    JzVec4 backgroundColor;
    JzVec4 textureColor;
};

} // namespace JzRE