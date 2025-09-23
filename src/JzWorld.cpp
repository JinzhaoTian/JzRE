/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzWorld.h"

void JzRE::JzWorld::Update(JzRE::F32 delta)
{
    for (const auto &system : m_systems) {
        system->Update(m_manager, delta);
    }
}