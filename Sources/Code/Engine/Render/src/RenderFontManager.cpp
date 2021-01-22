#include "RenderFontManager.hpp"
#include "RenderFont.hpp"
#include "Core/ETLogger.hpp"
#include "Core/ETAssets.hpp"
#include "Platform/OpenGL.hpp"
#include "Core/ETApplication.hpp"
#include "RenderConfig.hpp"
#include "RenderTexture.hpp"
#include "RenderUtils.hpp"

#include <algorithm>
#include <cassert>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace {

const int TEX_SUB_PADDING = 2;

} // namespace

RenderFontManager::RenderFontManager() {
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
    ETNode<ETRenderContextEvents>::connect(getEntityId());

    if(RenderUtils::IsOpenGLContextExists()) {
        ET_onContextCreated();
    }

    return true;
}

void RenderFontManager::deinit() {
    ETNode<ETRenderFontManager>::disconnect();
}

void RenderFontManager::ET_onContextCreated() {
    auto renderConfig = ET_getShared<RenderConfig>();
    auto res = createFont(renderConfig->defaultFont.c_str(), renderConfig->defaultFontSize);
    if(!res) {
        LogError("[RenderFontManager::ET_onContextCreated] Can't create default font");
    }
}

void RenderFontManager::ET_onContextDestroyed() {
}

std::shared_ptr<RenderFont> RenderFontManager::ET_getDefaultFont() {
    assert(RenderUtils::IsOpenGLContextExists() && "Can't get font without OpenGL context");
    auto renderConfig = ET_getShared<RenderConfig>();
    return createFont(renderConfig->defaultFont.c_str(), renderConfig->defaultFontSize);
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

    int fontHeight = 0;

    FT_GlyphSlot glyph = fontFace->glyph;
    for(auto ch : characterSet) {
        if(FT_Load_Char(fontFace, ch, FT_LOAD_NO_BITMAP)) {
            LogWarning("[RenderFontManager::createFontImpl] Failed to load character '%c'", ch);
            continue;
        }
        texWidth += glyph->bitmap.width + TEX_SUB_PADDING;
        texHeight = std::max(texHeight, glyph->bitmap.rows);
        fontHeight = std::max(fontHeight, static_cast<int>(glyph->bitmap.rows));
    }

    std::shared_ptr<RenderFont> font(new RenderFont(fontHeight));

    std::shared_ptr<RenderTexture> fontAtlas;
    ET_SendEventReturn(fontAtlas, &ETRenderTextureManger::ET_createTexture, ETextureType::R8);
    if(!fontAtlas) {
        LogWarning("[RenderFontManager::createFontImpl] Counld not create atlas for font: %s", fontName);
        FT_Done_Face(fontFace);
        FT_Done_FreeType(ftLib);
        return nullptr;
    }

    fontAtlas->bind();
    if(!fontAtlas->resizeAndClear(Vec2i(texWidth, texHeight))) {
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
        glyphData.texCoords.bot = Vec2(shift / static_cast<float>(texWidth), 0.f);
        glyphData.texCoords.top = Vec2((shift + glyph->bitmap.width) / static_cast<float>(texWidth),
            glyph->bitmap.rows / static_cast<float>(texHeight));

        if(glyphData.size > Vec2i(0)) {
            fontAtlas->writeR8(Vec2i(shift, 0), glyphData.size, glyph->bitmap.buffer);
            if(auto errStr = RenderUtils::GetGLError()) {
                LogError("[RenderFont::createFontImpl] Can't copy a glyph '%c' bitmap buffer to a font atlas (Error: %s)",
                    static_cast<char>(ch), errStr);
                FT_Done_Face(fontFace);
                FT_Done_FreeType(ftLib);
                return nullptr;
            }
        }

        font->addGlyph(ch, shift, glyphData);

        shift += glyph->bitmap.width + TEX_SUB_PADDING;
    }

    fontAtlas->unbind();
    font->setFontAtlas(fontAtlas);

    FT_Done_Face(fontFace);
    FT_Done_FreeType(ftLib);
    return font;
}
