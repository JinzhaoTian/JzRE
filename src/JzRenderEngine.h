/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzContext.h"
#include "JzEditor.h"

namespace JzRE {
/**
 * @brief Jz Render Engine
 */
class JzRenderEngine {
public:
    /**
     * @brief Constructor
     */
    JzRenderEngine();

    /**
     * @brief Destructor
     */
    ~JzRenderEngine();

    /**
     * @brief Run the render engine
     */
    void Run();

    /**
     * @brief Check if the render engine is running
     *
     * @return Bool
     */
    Bool IsRunning() const;

private:
    JzContext m_context;
    JzEditor  m_editor;
};
} // namespace JzRE