/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/Core/JzRETypes.h"
#include "JzRE/Core/JzEvent.h"
#include "JzRE/Core/JzVector.h"
#include "JzRE/RHI/JzRHITexture.h"
#include "JzRE/UI/JzWidget.h"

namespace JzRE {

/**
 * @brief Image Button Widget
 */
class JzImageButton : public JzWidget {
public:
    /**
     * @brief Constructor
     *
     * @param texture
     * @param size
     */
    JzImageButton(std::shared_ptr<JzRHITexture> texture, const JzVec2 &size);

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
    Bool   disabled = false;
    JzVec2 buttonSize;
    String buttonIdleColor;
    String buttonTextureColor;

private:
    std::shared_ptr<JzRHITexture> m_texture;
};

} // namespace JzRE