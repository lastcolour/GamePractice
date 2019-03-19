#include "Render/RenderFontSystem.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Platforms/OpenGL.hpp"

#include <algorithm>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace {
    const char* DEFAULT_FONT = "Render/Fonts/Shanti-Regular.ttf";
}

RenderFontSystem::RenderFontSystem() {
}

RenderFontSystem::~RenderFontSystem() {
}

std::shared_ptr<RenderFont> RenderFontSystem::createFont(const std::string& reqFontName, int fontSize) {
    auto it = fonts.find(reqFontName);
    if(it != fonts.end() && !it->second.expired()) {
        return it->second.lock();
    }
    buildFont(DEFAULT_FONT, fontSize);
    return nullptr;
}

void RenderFontSystem::buildFont(const std::string& fontName, int fontSize) {
    FT_Library ftLib;
    if(FT_Init_FreeType(&ftLib)) {
        LogError("[RenderFontSystem::buildFont] Can't init FreeType library");
        return;
    }
    Buffer buff;
    ET_SendEventReturn(buff, &ETAsset::ET_loadAsset, fontName);
    if(!buff) {
        LogError("[RenderFontSystem::buildFont] Can't load default font: %s", fontName);
        return;
    }
    FT_Face fontFace = nullptr;
    if(FT_New_Memory_Face(ftLib, static_cast<unsigned char*>(buff.getData()),
        static_cast<FT_Long>(buff.getSize()), 0, &fontFace)) {
        LogError("[RenderFontSystem::buildFont] Can't create memory font face for font: %s", fontName);
        return;
    }
    FT_Set_Pixel_Sizes(fontFace, 0, fontSize);

    FT_GlyphSlot glyph = fontFace->glyph;
    unsigned int width = 0;
    unsigned int height = 0;

    for(int i = 32; i < 128; ++i) {
        if(FT_Load_Char(fontFace, i, FT_LOAD_RENDER)) {
            LogWarning("[RenderFontSystem::buildFont] Failed to load character '%c'", i);
            continue;
        }
        width += glyph->bitmap.width;
        height = std::max(height, glyph->bitmap.rows);
    }

    GLuint texId;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);

    int shift = 0;
    for(int i = 32; i < 128; ++i) {
        if(FT_Load_Char(fontFace, i, FT_LOAD_RENDER)) {
            continue;
        }
        glTexSubImage2D(GL_TEXTURE_2D, 0, shift, 0, glyph->bitmap.width, glyph->bitmap.rows,
            GL_ALPHA, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);
        shift += glyph->bitmap.width;
    }
}
