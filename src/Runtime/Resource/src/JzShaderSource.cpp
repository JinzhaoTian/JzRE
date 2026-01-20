/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Resource/JzShaderSource.h"

#include <fstream>
#include <sstream>

namespace JzRE {

JzShaderSource::JzShaderSource(const String &path) :
    m_path(path)
{
    m_name  = path;
    m_state = JzEResourceState::Unloaded;
}

Bool JzShaderSource::Load()
{
    if (m_state == JzEResourceState::Loaded)
        return true;

    m_state = JzEResourceState::Loading;

    std::ifstream file(m_path);
    if (!file.is_open()) {
        m_state = JzEResourceState::Error;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    m_source = buffer.str();
    file.close();

    if (m_source.empty()) {
        m_state = JzEResourceState::Error;
        return false;
    }

    m_state = JzEResourceState::Loaded;
    return true;
}

void JzShaderSource::Unload()
{
    m_source.clear();
    m_state = JzEResourceState::Unloaded;
}

} // namespace JzRE
