/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/Runtime/Core/JzVector.h"
#include "JzRE/Runtime/Platform/JzGPUTextureObject.h"
#include "JzRE/Editor/UI/JzWidget.h"

namespace JzRE {

/**
 * @brief Image Widget
 */
class JzIcon : public JzWidget {
public:
    /**
     * @brief Constructor
     *
     * @param iconTexture
     * @param iconSize
     */
    JzIcon(std::shared_ptr<JzGPUTextureObject> iconTexture, const JzVec2 &iconSize = {16.f, 16.f});

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

private:
    JzVec2                              m_iconSize;
    std::shared_ptr<JzGPUTextureObject> m_iconTexture;
};

} // namespace JzRE