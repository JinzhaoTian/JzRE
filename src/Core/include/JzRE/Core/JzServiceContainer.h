/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <any>
#include <unordered_map>
#include "JzRE/Core/JzRETypes.h"

namespace JzRE {
/**
 * @brief Service Container
 */
class JzServiceContainer {
public:
    /**
     * @brief Init, clear all services
     */
    static void Init()
    {
        __SERVICES.clear();
    }

    /**
     * @brief Provide a service
     *
     * @tparam T The type of the service
     * @param service The service to provide
     */
    template <typename T>
    static void Provide(T &service)
    {
        __SERVICES[typeid(T).hash_code()] = std::any(&service);
    }

    /**
     * @brief Get a service
     *
     * @tparam T The type of the service
     * @return The service
     */
    template <typename T>
    static T &Get()
    {
        return *std::any_cast<T *>(__SERVICES[typeid(T).hash_code()]);
    }

private:
    static std::unordered_map<I32, std::any> __SERVICES;
};
} // namespace JzRE