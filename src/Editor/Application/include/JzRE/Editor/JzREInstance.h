/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include "JzRE/Runtime/JzRERuntime.h"
#include "JzRE/Editor/JzEditor.h"

namespace JzRE {

/**
 * @brief JzRE Editor Instance
 *
 * This class extends JzRERuntime to provide a complete editor experience.
 * It integrates the JzEditor UI with the runtime's rendering pipeline.
 *
 * The inheritance from JzRERuntime allows:
 * - Reusing all runtime functionality (window, device, renderer, scene, input)
 * - Injecting editor UI rendering via the OnRender hook
 * - Sharing the same architecture for both standalone and editor modes
 */
class JzREInstance : public JzRERuntime {
public:
    /**
     * @brief Constructor
     *
     * @param rhiType The RHI type to use for rendering
     * @param openDirectory The directory to open in the editor
     */
    JzREInstance(JzERHIType rhiType, std::filesystem::path &openDirectory);

    /**
     * @brief Destructor
     */
    ~JzREInstance() override;

    /**
     * @brief Get the editor instance
     *
     * @return JzEditor& Reference to the editor
     */
    JzEditor &GetEditor();

protected:
    /**
     * @brief Called before the main loop starts
     *
     * Initializes editor-specific components.
     */
    void OnStart() override;

    /**
     * @brief Called during the update phase for editor logic
     *
     * @param deltaTime Time elapsed since the last frame in seconds
     *
     * Updates editor logic (shortcuts, mode updates, panel updates).
     * Called before UpdateSystems() in the main loop.
     */
    void OnUpdate(F32 deltaTime) override;

    /**
     * @brief Called during the render phase for editor rendering
     *
     * @param deltaTime Time elapsed since the last frame in seconds
     *
     * Renders editor views and ImGui UI.
     * Called after UpdateSystems() and before Present().
     */
    void OnRender(F32 deltaTime) override;

    /**
     * @brief Called after the main loop ends
     *
     * Cleans up editor-specific components.
     */
    void OnStop() override;

private:
    std::filesystem::path     m_openDirectory;
    std::unique_ptr<JzEditor> m_editor;
};
} // namespace JzRE
