#pragma once

#include "CommonTypes.h"
#include "JzEvent.h"
#include "JzWidget.h"

namespace JzRE {

class JzButton : public JzWidget {
public:
    JzButton(const String &label = "",  Bool p_disabled = false);

protected:
    /**
     * @brief Implementation of the Draw method
     */
    void _Draw_Impl() override;

public:
    JzEvent<> ClickedEvent;

public:
    String label;
    Bool   disabled = false;
};
} // namespace JzRE