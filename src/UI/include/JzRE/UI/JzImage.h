/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRETypes.h"
#include "JzVector.h"
#include "JzWidget.h"

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