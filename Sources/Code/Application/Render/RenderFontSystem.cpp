#include "Render/RenderFontSystem.hpp"
#include "Render/RenderFont.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Platforms/OpenGL.hpp"

#include <algorithm>

#include <ft2build.h>
#include FT_FREETYPE_H

RenderFontSystem::RenderFontSystem() {
    characterSet = {
        'A', 'a',
        'B', 'b',
        'C', 'c',
        'D', 'd',
        'E', 'e',
        'F', 'f',
        'G', 'g',
        'H', 'h',
        'I', 'i',
        'J', 'j',
        'K', 'k',
        'L', 'l',
        'M', 'm',
        'N', 'n',
        'O', 'o',
        'p', 'p',
        'Q', 'q',
        'R', 'r',
        'S', 's',
        'T', 't',
        'U', 'u',
        'V', 'v',
        'W', 'w',
        'X', 'x',
        'Y', 'y',
        'Z', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        '+', '-', '*', '(', ')', '/', '%', '$', '[', ']', '{', '}', '<', '>', '=',
        '.', ',', ';', ':', '\'', '\\', '"', '@', '!', '?', '_'
    };
}

RenderFontSystem::~RenderFontSystem() {
}

std::shared_ptr<RenderFont> RenderFontSystem::createFont(const std::string& reqFontName, int fontSize) {
    std::string fontName = reqFontName + '_' + std::to_string(fontSize);
    auto it = fonts.find(fontName);
    if(it != fonts.end() && !it->second.expired()) {
        return it->second.lock();
    }
    auto font = createFontImpl(reqFontName, fontSize);
    if(font) {
        fonts[fontName] = font;
        return font;
    }
    return nullptr;
}

std::shared_ptr<RenderFont> RenderFontSystem::createFontImpl(const std::string& fontName, int fontSize) {
    FT_Library ftLib;
    if(FT_Init_FreeType(&ftLib)) {
        LogError("[RenderFontSystem::createFontImpl] Can't init FreeType library");
        return nullptr;
    }
    Buffer buff;
    ET_SendEventReturn(buff, &ETAsset::ET_loadAsset, fontName);
    if(!buff) {
        LogError("[RenderFontSystem::createFontImpl] Can't load default font: %s", fontName);
        return nullptr;
    }
    FT_Face fontFace = nullptr;
    if(FT_New_Memory_Face(ftLib, static_cast<unsigned char*>(buff.getData()),
        static_cast<FT_Long>(buff.getSize()), 0, &fontFace)) {
        LogError("[RenderFontSystem::createFontImpl] Can't create memory font face for font: %s", fontName);
        return nullptr;
    }

    FT_Set_Pixel_Sizes(fontFace, 0, fontSize);

    unsigned int width = 0;
    unsigned int height = 0;

    FT_GlyphSlot glyph = fontFace->glyph;
    for(auto ch : characterSet) {
        if(FT_Load_Char(fontFace, ch, FT_LOAD_RENDER)) {
            LogWarning("[RenderFontSystem::createFontImpl] Failed to load character '%c'", ch);
            continue;
        }
        width += glyph->bitmap.width;
        height = std::max(height, glyph->bitmap.rows);
        break;
    }

    std::shared_ptr<RenderFont> font(new RenderFont);
    if(!font->createAtlas(width, height)) {
        LogWarning("[RenderFontSystem::createFontImpl] Counld not create atlas for font: %s", fontName);
        return nullptr;
    }

    int shift = 0;
    for(auto ch : characterSet) {
        if(FT_Load_Char(fontFace, ch, FT_LOAD_RENDER)) {
            continue;
        }

        RenderGlyph glyphData;
        glyphData.advance.x = glyph->advance.x >> 6;
        glyphData.advance.y = glyph->advance.y >> 6;
        glyphData.size.x = glyph->bitmap.width;
        glyphData.size.y = glyph->bitmap.rows;
        glyphData.bearing.x = glyph->bitmap_left;
        glyphData.bearing.y = glyph->bitmap_top;
        glyphData.offset = shift;

        font->addGlyph(ch, glyphData, glyph->bitmap.buffer);

        shift += glyph->bitmap.width + 10;

        break;
    }
    return font;
}
