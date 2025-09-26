/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/Core/JzVector.h"
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
    JzImage(U32 p_textureId, const JzVec2 &p_size);

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

public:
    U32    textureId;
    JzVec2 size;
};
} // namespace JzRE