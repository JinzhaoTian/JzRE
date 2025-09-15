/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <unordered_map>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include <GLFW/glfw3.h>
#include <imgui.h>
#include "JzRETypes.h"
#include "JzCanvas.h"
#include "JzWindow.h"

namespace JzRE {

/**
 * @brief UI Manager of JzRE
 */
class JzUIManager {
public:
    /**
     * @brief Constructor
     */
    JzUIManager(JzWindow &window);

    /**
     * @brief Destructor
     */
    ~JzUIManager();

    /**
     * @brief Render
     */
    void Render();

    /**
     * @brief Set docking
     */
    void SetDocking(Bool value);

    /**
     * @brief Is docking
     *
     * @return Bool
     */
    Bool IsDocking() const;

    /**
     * @brief Set the Canvas
     *
     * @param canvas
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
     *
     * @param filename
     */
    void SetEditorLayoutSaveFilename(const String &filename);

    /**
     * @brief Reset layout
     *
     * @param configPath
     */
    void ResetLayout(const String &configPath) const;

    /**
     * @brief Load a font
     *
     * @param fontId
     * @param fontPath
     * @param fontSize
     * @return Bool
     */
    Bool LoadFont(const String &fontId, const String &fontPath, F32 fontSize);

    /**
     * @brief Unload a font (Returns true on success)
     *
     * @param fontId
     * @return Bool
     */
    Bool UnloadFont(const String &fontId);

    /**
     * @brief Set the given font as the current one
     *
     * @param fontId
     * @return Bool
     */
    Bool UseFont(const String &fontId);

    /**
     * @brief Use the default font
     */
    void UseDefaultFont();

private:
    Bool                                      m_dockingState;
    JzCanvas                                 *m_canvas             = nullptr;
    String                                    m_layoutSaveFilename = "imgui.ini";
    std::unordered_map<std::string, ImFont *> m_fonts;
};

} // namespace JzRE
