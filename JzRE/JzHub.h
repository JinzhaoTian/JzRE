#pragma once

#include "CommonTypes.h"
#include "JzRHIFactory.h"
#include "JzRHIDevice.h"
#include "JzWindow.h"
#include "JzWindowSettings.h"
#include "JzUIManager.h"
#include "JzCanvas.h"
#include "JzHubPanel.h"

namespace JzRE {

class JzHub {
public:
    /**
     * Constructor
     */
    JzHub();

    /**
     * Run the project hub logic
     */
    std::optional<std::filesystem::path> Run();

    /**
     * Setup the project hub specific context (minimalist context)
     */
    void SetupContext();

private:
    std::unique_ptr<JzRHIDevice> m_device;
    std::unique_ptr<JzWindow>    m_window;
    std::unique_ptr<JzUIManager> m_uiManager;
    JzCanvas                     m_canvas;
};

} // namespace JzRE