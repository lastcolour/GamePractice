#include "Render/RenderFont.hpp"
#include "Platforms/OpenGL.hpp"
#include "ETApplicationInterfaces.hpp"

#include <cassert>

RenderFont::RenderFont() :
    texSize(0),
    textureId(0) {
}

RenderFont::~RenderFont() {
}

const Vec2i& RenderFont::getTexSize() const {
    return texSize;
}

int RenderFont::getTexId() const {
    return textureId;
}

bool RenderFont::createAtlas(unsigned int width, unsigned int height) {
    if(textureId) {
        LogError("[RenderFont::createAtlas] Trying re-create font texture atlas");
        return false;
    }

    GLuint texId;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    Buffer buff(width * height / 2);
    memset(buff.getData(), 0, buff.getSize());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buff.getData());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    textureId = texId;
    texSize = Vec2i(width, height);

    return true;
}

void RenderFont::addGlyph(int ch, const RenderGlyph& glyphData, const void* buffer) {
    glyphs[ch] = glyphData;
    glTexSubImage2D(GL_TEXTURE_2D, 0, glyphData.texCoords.bot.x, 0, glyphData.size.x, glyphData.size.y,
        GL_RED, GL_UNSIGNED_BYTE, buffer);
}

const RenderGlyph* RenderFont::getGlyph(int ch) const {
    auto it = glyphs.find(ch);
    if(it != glyphs.end()) {
        return &it->second;
    }
    return nullptr;
}