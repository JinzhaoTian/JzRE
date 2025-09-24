/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */
#pragma once

#include <memory>
#include "JzWidget.h"
#include "JzEvent.h"
#include "JzRHITexture.h"
#include "JzVector.h"

namespace JzRE {
/**
 * @brief A button widget that displays an icon.
 */
class JzIconButton : public JzWidget {
public:
    /**
     * @brief Construct a new Jz Icon Button object.
     *
     * @param texture The texture to use as the icon.
     */
    JzIconButton(std::shared_ptr<JzRHITexture> texture);

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
    JzVec2 iconSize;
    JzVec4 idleBackgroundColor;
    JzVec4 hoveredBackgroundColor;
    JzVec4 clickedBackgroundColor;
    JzVec4 iconColor;
    JzVec4 hoveredIconColor;

private:
    std::shared_ptr<JzRHITexture> m_texture;
};
} // namespace JzRE
