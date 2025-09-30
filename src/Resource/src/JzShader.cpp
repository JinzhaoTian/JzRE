/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Resource/JzShader.h"
#include <fstream>
#include <sstream>
#include "JzRE/Core/JzServiceContainer.h"
#include "JzRE/RHI/JzDevice.h"

JzRE::JzShader::JzShader(const JzRE::String &path, JzRE::JzEShaderType type) :
    m_path(path),
    m_type(type)
{
    m_state = JzEResourceState::Unloaded;
}

JzRE::JzShader::~JzShader()
{
    Unload();
}

JzRE::Bool JzRE::JzShader::Load()
{
    if (m_state == JzEResourceState::Loaded) return true;
    m_state = JzEResourceState::Loading;

    std::ifstream file(m_path);
    if (!file.is_open()) {
        m_state = JzEResourceState::Error;
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    String source = buffer.str();
    file.close();

    if (source.empty()) {
        m_state = JzEResourceState::Error;
        return false;
    }

    auto        &device = JzServiceContainer::Get<JzDevice>();
    JzShaderDesc shaderDesc;
    shaderDesc.type      = m_type;
    shaderDesc.source    = source;
    shaderDesc.debugName = m_path;

    m_rhiShader = device.CreateShader(shaderDesc);

    if (m_rhiShader) {
        m_state = JzEResourceState::Loaded;
        return true;
    } else {
        m_state = JzEResourceState::Error;
        return false;
    }
}

void JzRE::JzShader::Unload()
{
    m_rhiShader = nullptr;
    m_state     = JzEResourceState::Unloaded;
}
