#pragma once

#include "CommonTypes.h"
#include "JzPanel.h"

namespace JzRE {

class JzPanelTransformable : public JzPanel {
public:
    /**
     * @brief Constructor
     */
    JzPanelTransformable();

    /**
     * @brief Destructor
     */
    virtual ~JzPanelTransformable() = default;
};

} // namespace JzRE