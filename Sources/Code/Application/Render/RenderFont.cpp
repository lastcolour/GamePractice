#include "Render/RenderFont.hpp"
#include "Platforms/OpenGL.hpp"
#include "ETApplicationInterfaces.hpp"

#include <algorithm>
#include <cassert>

RenderFont::RenderFont() :
    texSize(0),
    textureId(0),
    fontHeight(0) {
}

RenderFont::~RenderFont() {
    glDeleteTextures(1, &textureId);
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
    Buffer buff(width * height * 2);
    memset(buff.getWriteData(), 0, buff.getSize());
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buff.getWriteData());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    textureId = texId;
    texSize = Vec2i(width, height);

    auto errCode = glGetError();
    return errCode == GL_NO_ERROR;
}

void RenderFont::addGlyph(int ch, int shift, const RenderGlyph& glyphData, const void* buffer) {
    fontHeight = std::max(fontHeight, glyphData.size.y);
    glyphs[ch] = glyphData;
    glTexSubImage2D(GL_TEXTURE_2D, 0, shift, 0, glyphData.size.x, glyphData.size.y,
        GL_RED, GL_UNSIGNED_BYTE, buffer);
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