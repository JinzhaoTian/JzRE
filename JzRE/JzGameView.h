#pragma once

#include "CommonTypes.h"
#include "JzCamera.h"
#include "JzEditorActions.h"
#include "JzScene.h"
#include "JzSceneManager.h"
#include "JzView.h"

namespace JzRE {
/**
 * @brief Game View Panel Window
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