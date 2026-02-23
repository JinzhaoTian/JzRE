/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>

#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"
#include "JzRE/Editor/UI/JzWidget.h"

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
    std::shared_ptr<JzGPUTextureObject> frameTexture;
    void  *frameTextureId = nullptr;
    JzVec2 frameSize;
};

} // namespace JzRE
