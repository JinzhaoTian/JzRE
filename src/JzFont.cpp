/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzFont.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "JzContext.h"
#include "JzRHIDevice.h"
#include "JzRHIDescription.h"

JzRE::JzFont::JzFont(const JzRE::String &path, JzRE::U32 size) :
    m_path(path),
    m_size(size)
{
    m_state = JzEResourceState::Unloaded;
}

JzRE::JzFont::~JzFont()
{
    Unload();
}

JzRE::Bool JzRE::JzFont::Load()
{
    if (m_state == JzEResourceState::Loaded) return true;
    m_state = JzEResourceState::Loading;

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        m_state = JzEResourceState::Error;
        return false;
    }

    FT_Face face;
    if (FT_New_Face(ft, m_path.c_str(), 0, &face)) {
        FT_Done_FreeType(ft);
        m_state = JzEResourceState::Error;
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, m_size);

    // A more optimal approach is to create a single large texture atlas.
    auto &device = JzRE_DEVICE();

    for (unsigned char c = 0; c < 128; c++) { // Only load ASCII characters for now
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            // log error
            continue;
        }

        JzTextureDesc textureDesc;
        textureDesc.width  = face->glyph->bitmap.width;
        textureDesc.height = face->glyph->bitmap.rows;
        textureDesc.format = JzETextureFormat::R8; // Glyphs are single-channel
        textureDesc.data   = face->glyph->bitmap.buffer;

        auto rhiTexture = device.CreateTexture(textureDesc);

        Character character = {
            std::make_shared<JzTexture>(rhiTexture), // Create a JzTexture wrapper
            {(int)face->glyph->bitmap.width, (int)face->glyph->bitmap.rows},
            {face->glyph->bitmap_left, face->glyph->bitmap_top},
            (uint32_t)face->glyph->advance.x};
        m_characters.insert({c, character});
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    m_state = JzEResourceState::Loaded;
    return true;
}

void JzRE::JzFont::Unload()
{
    m_characters.clear();
    m_state = JzEResourceState::Unloaded;
}
