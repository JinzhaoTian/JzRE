/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/CLI/JzCliContext.h"

#include "JzRE/Runtime/Core/JzServiceContainer.h"

namespace JzRE {

Bool JzCliContext::Initialize()
{
    if (m_initialized) {
        return true;
    }

    JzServiceContainer::Init();
    JzServiceContainer::Provide<JzProjectManager>(m_projectManager);

    m_initialized = true;
    return true;
}

void JzCliContext::Shutdown()
{
    if (!m_initialized) {
        return;
    }

    m_projectManager.CloseProject();
    JzServiceContainer::Init();
    m_initialized = false;
}

JzEProjectResult JzCliContext::LoadProject(const std::filesystem::path &projectPath)
{
    return m_projectManager.LoadProject(projectPath);
}

JzProjectManager &JzCliContext::GetProjectManager()
{
    return m_projectManager;
}

JzAssetImporter &JzCliContext::GetAssetImporter()
{
    return m_assetImporter;
}

JzAssetExporter &JzCliContext::GetAssetExporter()
{
    return m_assetExporter;
}

} // namespace JzRE
