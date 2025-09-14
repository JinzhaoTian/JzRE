/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <filesystem>
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
    JzRenderEngine(JzERHIType rhiType, std::filesystem::path &openDirectory);

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
    std::unique_ptr<JzEditor> m_editor;
};
} // namespace JzRE