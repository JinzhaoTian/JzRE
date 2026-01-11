/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <map>
#include "JzRE/Runtime/Resource/JzResource.h"
#include "JzRE/Runtime/Resource/JzTexture.h"
#include "JzRE/Runtime/Core/JzVector.h"

namespace JzRE {

/**
 * @brief Represents a font asset, holding character glyphs and metrics.
 */
class JzFont : public JzResource {
public:
    /**
     * @brief Holds all relevant information about a single character glyph.
     */
    struct Character {
        std::shared_ptr<JzTexture> texture; // Texture for this single glyph
        JzIVec2                    size;    // Size of glyph
        JzIVec2                    bearing; // Offset from baseline to left/top of glyph
        U32                        advance; // Horizontal offset to advance to next glyph
    };

    /**
     * @brief Constructor
     *
     * @param path
     * @param size
     */
    JzFont(const String &path, U32 size);

    /**
     * @brief Destructor
     */
    virtual ~JzFont();

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
     * @brief Get the Character object
     *
     * @param c
     *
     * @return const Character&
     */
    const Character &GetCharacter(char c) const
    {
        return m_characters.at(c);
    }

private:
    String                    m_path;
    U32                       m_size;
    std::map<char, Character> m_characters;
};

} // namespace JzRE
