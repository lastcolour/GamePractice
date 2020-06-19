#include "RenderFont.hpp"
#include "Platforms/OpenGL.hpp"
#include "Core/ETLogger.hpp"
#include "RenderTexture.hpp"
#include "RenderUtils.hpp"
#include "Render/ETRenderManager.hpp"

#include <algorithm>
#include <cassert>

RenderFont::RenderFont() :
    fontHeight(0) {
}

RenderFont::~RenderFont() {
}

const Vec2i& RenderFont::getTexSize() const {
    return tex->size;
}

int RenderFont::getTexId() const {
    if(tex) {
        return tex->texId;
    }
    return 0;
}

bool RenderFont::createAtlas(unsigned int width, unsigned int height) {
    if(tex) {
        LogError("[RenderFont::createAtlas] Trying re-create font texture atlas");
        return false;
    }
    ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createEmptyTexture, Vec2i(width, height), ETextureType::R8);
    if(!tex) {
        return false;
    }
    return true;
}

void RenderFont::addGlyph(int ch, int shift, const RenderGlyph& glyphData, const void* buffer) {
    fontHeight = std::max(fontHeight, glyphData.size.y);
    glyphs[ch] = glyphData;
    if(glyphData.size > Vec2i(0)) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, shift, 0, glyphData.size.x, glyphData.size.y,
            GL_RED, GL_UNSIGNED_BYTE, buffer);
        if(!CheckGLError()) {
            LogError("[RenderFont::addGlyph] Can't add glyph '%c' to font atlas", static_cast<char>(ch));
        }
    }
}

int RenderFont::getHeight() const {
    return fontHeight;
}

const RenderGlyph* RenderFont::getGlyph(int ch) const {
    auto it = glyphs.find(ch);
    if(it != glyphs.end()) {
        return &it->second;
    }
    return nullptr;
}