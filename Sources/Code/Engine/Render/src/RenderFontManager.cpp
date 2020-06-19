#include "RenderFontManager.hpp"
#include "RenderFont.hpp"
#include "Core/ETLogger.hpp"
#include "Core/ETAssets.hpp"
#include "Platforms/OpenGL.hpp"

#include <algorithm>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace {

const char* DEF_FONT_NAME = "Render/Fonts/Shanti-Regular.ttf";
const int DEF_FONT_SIZE = 38;

} // namespace

RenderFontManager::RenderFontManager() :
    padding(2) {
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
        'P', 'p',
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
        '.', ',', ';', ':', '\'', '\\', '"', '@', '!', '?', '_', '#', '&', '^', '`', '~', '|',
        ' '
    };
}

RenderFontManager::~RenderFontManager() {
}

bool RenderFontManager::init() {
    ETNode<ETRenderFontManager>::connect(getEntityId());
    ETNode<ETRenderResourceManager>::connect(getEntityId());
    return true;
}

void RenderFontManager::deinit() {
    ETNode<ETRenderFontManager>::disconnect();
    ETNode<ETRenderResourceManager>::disconnect();
}

std::shared_ptr<RenderFont> RenderFontManager::ET_createDefaultFont() {
    return createFont(DEF_FONT_NAME, DEF_FONT_SIZE);
}

std::shared_ptr<RenderFont> RenderFontManager::ET_createFont(const char* fontName) {
    return createFont(fontName, DEF_FONT_SIZE);
}

std::shared_ptr<RenderFont> RenderFontManager::createFont(const char* reqFontName, int fontSize) {
    std::string fontName = reqFontName;
    fontName += '_' + std::to_string(fontSize);
    auto it = fonts.find(fontName);
    if(it != fonts.end()) {
        return it->second;
    }
    auto font = createFontImpl(reqFontName, fontSize);
    if(font) {
        fonts[fontName] = font;
        return font;
    }
    return nullptr;
}

std::shared_ptr<RenderFont> RenderFontManager::createFontImpl(const char* fontName, int fontSize) {
    FT_Library ftLib;
    if(FT_Init_FreeType(&ftLib)) {
        LogError("[RenderFontManager::createFontImpl] Can't init FreeType library");
        return nullptr;
    }
    Buffer buff;
    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, fontName);
    if(!buff) {
        FT_Done_FreeType(ftLib);
        LogError("[RenderFontManager::createFontImpl] Can't load default font: %s", fontName);
        return nullptr;
    }
    FT_Face fontFace = nullptr;
    if(FT_New_Memory_Face(ftLib, static_cast<unsigned char*>(buff.getWriteData()),
        static_cast<FT_Long>(buff.getSize()), 0, &fontFace)) {
        FT_Done_FreeType(ftLib);
        LogError("[RenderFontManager::createFontImpl] Can't create memory font face for font: %s", fontName);
        return nullptr;
    }

    FT_Set_Pixel_Sizes(fontFace, 0, fontSize);

    unsigned int texWidth = 0;
    unsigned int texHeight = 0;

    FT_GlyphSlot glyph = fontFace->glyph;
    for(auto ch : characterSet) {
        if(FT_Load_Char(fontFace, ch, FT_LOAD_RENDER)) {
            LogWarning("[RenderFontManager::createFontImpl] Failed to load character '%c'", ch);
            continue;
        }
        texWidth += glyph->bitmap.width + padding;
        texHeight = std::max(texHeight, glyph->bitmap.rows);
    }

    std::shared_ptr<RenderFont> font(new RenderFont);
    if(!font->createAtlas(texWidth, texHeight)) {
        LogWarning("[RenderFontManager::createFontImpl] Counld not create atlas for font: %s", fontName);
        FT_Done_Face(fontFace);
        FT_Done_FreeType(ftLib);
        return nullptr;
    }

    glBindTexture(GL_TEXTURE_2D, font->getTexId());

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
        glyphData.texCoords.bot = Vec2(shift / static_cast<float>(texWidth), 0.f);
        glyphData.texCoords.top = Vec2((shift + glyph->bitmap.width) / static_cast<float>(texWidth),
            glyph->bitmap.rows / static_cast<float>(texHeight));

        font->addGlyph(ch, shift, glyphData, glyph->bitmap.buffer);

        shift += glyph->bitmap.width + padding;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(fontFace);
    FT_Done_FreeType(ftLib);
    return font;
}

void RenderFontManager::ET_forgetResoruces() {
    fonts.clear();
}

void RenderFontManager::ET_cleanUnused() {
}
