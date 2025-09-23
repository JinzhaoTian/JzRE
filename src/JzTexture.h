/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzResource.h"
#include "JzRHITexture.h"

namespace JzRE {

/**
 * @brief Texture resource
 */
class JzTexture : public JzResource {
public:
    /**
     * @brief Constructor
     *
     * @param name
     */
    JzTexture(const String &name);

    /**
     * @brief Destructor
     */
    virtual ~JzTexture() = default;

    /**
     * @brief Load
     *
     * @return Bool
     */
    virtual Bool Load() override;

    /**
     * @brief Unload
     */
    virtual void Unload() override;

    /**
     * @brief Get RHI texture
     *
     * @return JzRHITexture*
     */
    JzRHITexture *GetRHITexture() const
    {
        return m_rhiTexture.get();
    }

private:
    std::unique_ptr<JzRHITexture> m_rhiTexture;
};

} // namespace JzRE
