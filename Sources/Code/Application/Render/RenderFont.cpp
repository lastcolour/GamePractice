#include "Render/RenderFont.hpp"
#include "Platforms/OpenGL.hpp"
#include "ETApplicationInterfaces.hpp"

RenderFont::RenderFont() :
    textureId(0),
    texSize(0) {
}

RenderFont::~RenderFont() {
}

const Vec2i& RenderFont::getAtlasTexSize() const {
    return texSize;
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    textureId = texId;
    texSize = Vec2i(width, height);

    return true;
}

void RenderFont::addGlyph(int ch, const RenderGlyph& glyphData, const void* buffer) {
    glyphs[ch] = glyphData;
    glTexSubImage2D(GL_TEXTURE_2D, 0, glyphData.offset, 0, glyphData.size.x, glyphData.size.y,
        GL_ALPHA, GL_UNSIGNED_BYTE, buffer);
}

const RenderGlyph* RenderFont::getGlyph(int ch) const {
    auto it = glyphs.find(ch);
    if(it != glyphs.end()) {
        return &it->second;
    }
    return nullptr;
}