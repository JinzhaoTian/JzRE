#pragma once

#include "CommonTypes.h"
#include "JzCanvas.h"

namespace JzRE {
/**
 * @brief UI Manager
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

    void EnableEditorLayoutSave(Bool value);

    Bool IsEditorLayoutSaveEnabled() const;

    void SetEditorLayoutSaveFilename(const String &filename);

    void ResetLayout(const String &configPath) const;

private:
    Bool      m_dockingState;
    JzCanvas *m_canvas             = nullptr;
    String    m_layoutSaveFilename = "imgui.ini";
};
} // namespace JzRE
