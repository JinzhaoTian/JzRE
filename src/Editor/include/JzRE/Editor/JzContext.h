/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include <atomic>
#include "JzRE/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief Context of JzRE
 */
class JzContext {
public:
    /**
     * @brief Get singleton instance Context
     *
     * @return JzContext&
     */
    static JzContext &GetInstance();

    /**
     * @brief Initialize Context
     *
     * @param openDirectory Current open directory
     *
     * @return Bool
     */
    Bool Initialize(std::filesystem::path &openDirectory);

    /**
     * @brief Is Context initialized
     *
     * @return Bool
     */
    Bool IsInitialized() const;

    /**
     * @brief Get the Current Project Path
     *
     * @return std::filesystem::path
     */
    std::filesystem::path GetCurrentPath() const;

private:
    JzContext()                             = default;
    ~JzContext()                            = default;
    JzContext(const JzContext &)            = delete;
    JzContext &operator=(const JzContext &) = delete;

private:
    std::filesystem::path m_workDirectory;
    std::filesystem::path m_openDirectory;
    std::atomic<Bool>     m_running;
};

} // namespace JzRE
