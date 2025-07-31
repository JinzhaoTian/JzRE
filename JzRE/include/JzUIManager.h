#pragma once

#include "CommonTypes.h"
#include "JzCanvas.h"

namespace JzRE {
/**
 * @brief UI Manager of JzRE
 */
class JzUIManager {
public:
    /**
     * @brief Constructor
     */
    JzUIManager(GLFWwindow *glfwWindow, const String &glslVersion = "#version 150");

    /**
     * @brief Destructor
     */
    ~JzUIManager();

    /**
     * @brief Set docking
     */
    void SetDocking(Bool value);

    /**
     * @brief Get docking
     */
    Bool IsDocking() const;

    /**
     * @brief Render
     */
    void Render();

    /**
     * @brief Set canvas
     */
    void SetCanvas(JzCanvas &canvas);

    /**
     * @brief Remove canvas
     */
    void RemoveCanvas();

    /**
     * @brief Enable editor layout save
     */
    void EnableEditorLayoutSave(Bool value);

    /**
     * @brief Check if editor layout save is enabled
     */
    Bool IsEditorLayoutSaveEnabled() const;

    /**
     * @brief Set editor layout save filename
     */
    void SetEditorLayoutSaveFilename(const String &filename);

    /**
     * @brief Reset layout
     */
    void ResetLayout(const String &configPath) const;

private:
    Bool      m_dockingState;
    JzCanvas *m_canvas             = nullptr;
    String    m_layoutSaveFilename = "imgui.ini";
};
} // namespace JzRE
