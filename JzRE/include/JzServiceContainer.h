#pragma once

#include "CommonTypes.h"

namespace JzRE {
/**
 * @brief Service locator
 */
class JzServiceContainer {
public:
    /**
     * @brief Provide a service
     */
    template <typename T>
    static void Provide(T &p_service)
    {
        __SERVICES[typeid(T).hash_code()] = std::any(&p_service);
    }

    /**
     * @brief Get a service
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