#pragma once

#include "CommonTypes.h"
#include "JzCamera.h"
#include "JzISerializable.h"

namespace JzRE {
/**
 * @brief Scene
 */
class JzScene : public JzISerializable {
public:
    /**
     * @brief Construct a new JzScene object
     */
    JzScene();

    /**
     * @brief Destroy the JzScene object
     */
    ~JzScene();

    JzCamera *FindMainCamera() const;

    /**
     * @brief Serialize the scene
     *
     * @param filePath
     */
    virtual void Serialize(const String &filePath) override;

    /**
     * @brief Deserialize the scene
     *
     * @param filePath
     */
    virtual void Deserialize(const String &filePath) override;

    /** */
};
} // namespace JzRE