#include "RenderFontManager.hpp"
#include "RenderFont.hpp"
#include "Core/ETAssets.hpp"
#include "Core/ETApplication.hpp"
#include "RenderTexture.hpp"
#include "RenderUtils.hpp"
#include "RenderConfig.hpp"
#include "Core/TimePoint.hpp"

#include <cassert>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace {

const int TEX_SUB_PADDING = 2;

void writeBitmapToBuffer(Memory::Buffer& buff, const Vec2i& size, const Vec2i& pt, const Vec2i& subSize, uint8_t* data) {
    uint8_t* ptr = static_cast<uint8_t*>(buff.getWriteData());
    for(int q = 0, j = pt.y; q < subSize.y; ++j, ++q) {
        for(int p = 0, i = pt.x; p < subSize.x; ++i, ++p) {
            ptr[j * size.x + i] = data[q * subSize.x + p];
        }
    }
}

void writeMonochromeBitmapToBuffer(Memory::Buffer& buff, const Vec2i& size, const Vec2i& pt, Vec2i subSize, int pitch, uint8_t* data) {
    uint8_t* ptr = static_cast<uint8_t*>(buff.getWriteData());
    for(int q = 0, j = pt.y; q < subSize.y; ++j, ++q) {
        uint8_t* bytePtr = data + q * pitch;
        int bitIdx = 0;
        for(int p = 0, i = pt.x; p < subSize.x; ++i, ++p) {
            ptr[j * size.x + i] = (*bytePtr) & (1 << (7 - (bitIdx++))) ? 255: 0;
            if(bitIdx == 8) {
                ++bytePtr;
                bitIdx = 0;
            }
        }
    }
}

std::shared_ptr<RenderTexture> createFontAtlas(Memory::Buffer& buff, const Vec2i& size, const FontDescription& fontDescr) {
    std::shared_ptr<RenderTexture> fontAtlas;
    ET_SendEventReturn(fontAtlas, &ETRenderTextureManager::ET_createTexture, ETextureDataType::R8, size);
    if(!fontAtlas) {
        return nullptr;
    }
    fontAtlas->bind();
    fontAtlas->writeR8(Vec2i(0), size, buff.getReadData());
    if(auto errStr = RenderUtils::GetGLError()) {
        fontAtlas->unbind();
        LogError("[RenderFont::createFontImpl] Can't copy buffer to a texutre (Error: %s)", errStr);
        return nullptr;
    }
    fontAtlas->setLerpType(fontDescr.lerpType, fontDescr.lerpType);
    fontAtlas->unbind();
    return fontAtlas;
}

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
    auto renderConfig = Core::GetGlobal<RenderConfig>();
    if(!createFont(renderConfig->fontsConfig.gameFont)) {
        LogError("[RenderFontManager::ET_onContextCreated] Can't create 'Game' font");
    }
    if(!createFont(renderConfig->fontsConfig.debugFont)) {
        LogError("[RenderFontManager::ET_onContextCreated] Can't create 'Debug' font");
    }
}

void RenderFontManager::ET_onContextDestroyed() {
}

std::shared_ptr<RenderFont> RenderFontManager::ET_createFont(EFontType fontType) {
    auto renderConfig = Core::GetGlobal<RenderConfig>();
    switch(fontType) {
        case EFontType::Game: {
            return createFont(renderConfig->fontsConfig.gameFont);
        }
        case EFontType::Debug: {
            return createFont(renderConfig->fontsConfig.debugFont);
        }
        default: {
            assert(false && "Invalid font type");
        }
    }
    return nullptr;
}

std::shared_ptr<RenderFont> RenderFontManager::createFont(const FontDescription& fontDescr) {
    std::string fontName = fontDescr.file +  '_' + std::to_string(fontDescr.size);
    auto it = fonts.find(fontName);
    if(it != fonts.end()) {
        return it->second;
    }

    auto createStartT = TimePoint::GetNowTime();

    auto font = createFontImpl(fontDescr);
    if(!font) {
        return nullptr;
    }

    float msValue = -createStartT.getMiliSecElapsedFrom(TimePoint::GetNowTime());
    LogDebug("[RenderFontManager::createFont] Font '%s' created in %.1f ms", fontName, msValue);

    fonts[std::move(fontName)] = font;
    return font;
}

std::shared_ptr<RenderFont> RenderFontManager::createFontImpl(const FontDescription& fontDescr) {
    assert(RenderUtils::IsOpenGLContextExists() && "Can't create font without OpenGL context");

    FT_Library ftLib;
    if(FT_Init_FreeType(&ftLib)) {
        LogError("[RenderFontManager::createFontImpl] Can't init FreeType library");
        return nullptr;
    }
    Memory::Buffer fontBuff;
    ET_SendEventReturn(fontBuff, &ETAssets::ET_loadAsset, fontDescr.file.c_str());
    if(!fontBuff) {
        FT_Done_FreeType(ftLib);
        LogError("[RenderFontManager::createFontImpl] Can't load font from: '%s'",
            fontDescr.file);
        return nullptr;
    }
    FT_Face fontFace = nullptr;
    if(FT_New_Memory_Face(ftLib, static_cast<unsigned char*>(fontBuff.getWriteData()),
        static_cast<FT_Long>(fontBuff.getSize()), 0, &fontFace)) {
        FT_Done_FreeType(ftLib);
        LogError("[RenderFontManager::createFontImpl] Can't create memory font face for font: '%s'",
            fontDescr.file);
        return nullptr;
    }

    assert(fontDescr.size > 0 && "Negative font size");

    FT_Set_Pixel_Sizes(fontFace, 0, fontDescr.size);

    Vec2i texSize(0);

    FT_GlyphSlot glyph = fontFace->glyph;
    for(auto ch : characterSet) {
        if(FT_Load_Char(fontFace, ch, FT_LOAD_NO_BITMAP)) {
            LogWarning("[RenderFontManager::createFontImpl] Failed to load character '%c'", ch);
            continue;
        }
        texSize.x += glyph->bitmap.width + TEX_SUB_PADDING;
        texSize.y = std::max(texSize.y, static_cast<int>(glyph->bitmap.rows));
    }

    Memory::Buffer texBuff;
    texBuff.resize(texSize.x * texSize.y);
    memset(texBuff.getWriteData(), 0, texBuff.getSize());

    int fontHeight = texSize.y;
    std::shared_ptr<RenderFont> font(new RenderFont(fontHeight));

    int ftRenderFlag = FT_LOAD_RENDER;
    if(fontDescr.monochrome) {
        ftRenderFlag |= FT_LOAD_TARGET_MONO;
    }

    int shift = 0;
    for(auto ch : characterSet) {
        if(FT_Load_Char(fontFace, ch, ftRenderFlag)) {
            continue;
        }

        RenderGlyph glyphData;
        glyphData.advance.x = glyph->advance.x >> 6;
        glyphData.advance.y = glyph->advance.y >> 6;
        glyphData.size.x = glyph->bitmap.width;
        glyphData.size.y = glyph->bitmap.rows;
        glyphData.bearing.x = glyph->bitmap_left;
        glyphData.bearing.y = glyph->bitmap_top;
        glyphData.texCoords.bot = Vec2(shift / static_cast<float>(texSize.x), 0.f);
        glyphData.texCoords.top = Vec2((shift + glyph->bitmap.width) / static_cast<float>(texSize.x),
            glyph->bitmap.rows / static_cast<float>(texSize.y));

        if(glyphData.size > Vec2i(0)) {
            if(fontDescr.monochrome) {
                writeMonochromeBitmapToBuffer(texBuff, texSize, Vec2i(shift, 0),
                    glyphData.size, glyph->bitmap.pitch, glyph->bitmap.buffer);
            } else {
                writeBitmapToBuffer(texBuff, texSize, Vec2i(shift, 0), glyphData.size, glyph->bitmap.buffer);
            }
        }

        font->addGlyph(ch, shift, glyphData);
        shift += glyph->bitmap.width + TEX_SUB_PADDING;
    }

    FT_Done_Face(fontFace);
    FT_Done_FreeType(ftLib);

    auto fontAtlas = createFontAtlas(texBuff, texSize, fontDescr);
    if(!fontAtlas) {
        LogError("[RenderFontManager::createFontImpl] Can't create font atlas for a font: '%s'",
            fontDescr.file);
        return nullptr;
    }
    font->setFontAtlas(fontAtlas);
    return font;
}
