/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Editor/Core/JzEvent.h"
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"
#include "JzRE/Editor/UI/JzWidget.h"

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
    JzImageButton(std::shared_ptr<JzGPUTextureObject> texture, const JzVec2 &size);

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
    std::shared_ptr<JzGPUTextureObject> m_texture;
};

} // namespace JzRE
