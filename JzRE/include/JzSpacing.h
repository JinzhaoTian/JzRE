#pragma once

#include "CommonTypes.h"
#include "JzWidget.h"

namespace JzRE {

class JzSpacing : public JzWidget {
public:
    /**
     * Constructor
     * @param p_spaces
     */
    JzSpacing(U16 spaces = 1);

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

public:
    U16 spaces = 1;
};
} // namespace JzRE