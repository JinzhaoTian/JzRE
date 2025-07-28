#pragma once

#include "CommonTypes.h"
#include "JzView.h"

namespace JzRE {
/**
 * @brief Game View
 */
class JzGameView : public JzView {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param is_opened
     */
    JzGameView(const String &name, Bool is_opened);
};
} // namespace JzRE