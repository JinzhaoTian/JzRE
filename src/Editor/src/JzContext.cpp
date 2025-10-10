/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/JzContext.h"

JzRE::JzContext &JzRE::JzContext::GetInstance()
{
    static JzContext instance;
    return instance;
}

JzRE::Bool JzRE::JzContext::Initialize(std::filesystem::path &openDirectory)
{
    m_workDirectory = std::filesystem::current_path();
    m_openDirectory = openDirectory;
    m_running       = true;

    return true;
}

JzRE::Bool JzRE::JzContext::IsInitialized() const
{
    return false;
}

std::filesystem::path JzRE::JzContext::GetCurrentPath() const
{
    return m_openDirectory;
}