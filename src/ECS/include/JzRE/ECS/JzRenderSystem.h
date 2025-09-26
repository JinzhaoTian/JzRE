/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/RHI/JzRHIDevice.h"
#include "JzRE/ECS/JzSystem.h"

namespace JzRE {

/**
 * @brief The system responsible for rendering entities.
 */
class JzRenderSystem : public JzSystem {
public:
    /**
     * @brief Constructor.
     *
     * @param device A shared pointer to the RHI device.
     */
    JzRenderSystem(std::shared_ptr<JzRHIDevice> device);

    /**
     * @brief Destructor
     */
    virtual ~JzRenderSystem() = default;

    /**
     * @brief Updates and renders all visible entities.
     *
     * @param manager The entity manager to query entities from.
     * @param delta The delta time since the last frame.
     */
    virtual void Update(JzEntityManager &manager, F32 delta) override;

private:
    std::shared_ptr<JzRHIDevice> m_device;
};

} // namespace JzRE
