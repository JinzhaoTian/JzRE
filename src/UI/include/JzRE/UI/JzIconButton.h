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
 * @brief A button widget that displays an icon.
 */
class JzIconButton : public JzWidget {
public:
    /**
     * @brief Construct a new Jz Icon Button object.
     *
     * @param iconTexture The texture to use as the icon.
     */
    JzIconButton(std::shared_ptr<JzRHITexture> iconTexture);

protected:
    /**
     * @brief The implementation of the drawing logic.
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
    String buttonHoveredColor;
    String buttonClickedColor;
    JzVec2 iconSize;
    String iconIdleColor;
    String iconHoveredColor;

private:
    std::shared_ptr<JzRHITexture> m_iconTexture;
};

} // namespace JzRE
