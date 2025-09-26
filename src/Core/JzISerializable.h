/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRETypes.h"

namespace JzRE {
/**
 * @brief Interface for Serializable Objects
 */
class JzISerializable {
public:
    /**
     * @brief Serialize the object
     *
     * @param filePath
     */
    virtual void Serialize(const String &filePath) = 0;

    /**
     * @brief Deserialize the object
     *
     * @param filePath
     */
    virtual void Deserialize(const String &filePath) = 0;

    /**
     * @brief Destructor
     */
    virtual ~JzISerializable() = default;
};
} // namespace JzRE