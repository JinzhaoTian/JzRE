/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzWorld.h"

namespace JzRE {

template <typename T>
std::shared_ptr<T> JzWorld::RegisterSystem()
{
    auto system = std::make_shared<T>();
    m_systems.push_back(system);
    return system;
}

} // namespace JzRE
