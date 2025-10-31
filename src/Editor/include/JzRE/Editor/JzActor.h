/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include <vector>

#include "JzRE/Editor/JzObject.h"
#include "JzRE/Editor/JzActorComponent.h"

namespace JzRE {

/**
 * @brief Base class for all actors in JzRE
 */
class JzActor : public JzObject {
public:
    /**
     * @brief Adds a component of a specific type to the actor.
     *
     * @tparam T The type of the component to add. Must be a subclass of JzActorComponent.
     * @tparam TArgs The types of the arguments for the component's constructor.
     *
     * @param args The arguments to forward to the component's constructor.
     *
     * @return A pointer to the newly created component.
     */
    template <typename T, typename... TArgs>
    T *AddComponent(TArgs &&...args)
    {
        static_assert(std::is_base_of_v<JzActorComponent, T>, "T must be a JzActorComponent");
        auto new_component = std::make_unique<T>(std::forward<TArgs>(args)...);
        new_component->SetOwner(this);
        m_components.push_back(std::move(new_component));
        return static_cast<T *>(m_components.back().get());
    }

    /**
     * @brief Gets a component of a specific type.
     *
     * @tparam T The type of the component to get.
     *
     * @return A pointer to the component if found, otherwise nullptr.
     */
    template <typename T>
    T *GetComponent() const
    {
        static_assert(std::is_base_of_v<JzActorComponent, T>, "T must be a JzActorComponent");
        for (const auto &component : m_components) {
            if (auto *casted_component = dynamic_cast<T *>(component.get())) {
                return casted_component;
            }
        }
        return nullptr;
    }

    /**
     * @brief Gets all components of a specific type.
     *
     * @tparam T The type of the components to get.
     *
     * @return A vector of pointers to the components of the specified type.
     */
    template <typename T>
    std::vector<T *> GetComponents() const
    {
        static_assert(std::is_base_of_v<JzActorComponent, T>, "T must be a JzActorComponent");
        std::vector<T *> result;
        for (const auto &component : m_components) {
            if (auto *casted_component = dynamic_cast<T *>(component.get())) {
                result.push_back(casted_component);
            }
        }
        return result;
    }

private:
    std::vector<std::unique_ptr<JzActorComponent>> m_components;
};

} // namespace JzRE
