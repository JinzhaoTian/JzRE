/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Core/JzRETypes.h"
#include "JzRE/Editor/JzEditor.h"

namespace JzRE {
/**
 * @brief Jz Render Engine
 */
class JzREInstance {
public:
    /**
     * @brief Constructor
     */
    JzREInstance(JzERHIType rhiType, std::filesystem::path &openDirectory);

    /**
     * @brief Destructor
     */
    ~JzREInstance();

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