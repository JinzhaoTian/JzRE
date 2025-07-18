#pragma once

#include "CommonTypes.h"

namespace JzRE {
/**
 * @brief Serializable interface
 */
class IJzSerializable {
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
};
} // namespace JzRE