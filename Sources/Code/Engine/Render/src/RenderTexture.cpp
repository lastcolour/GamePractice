#include "RenderTexture.hpp"
#include "Platform/OpenGL.hpp"

#include <cassert>

namespace {

GLenum getGLFilterType(TexLerpType lerpType) {
    GLenum glLerpType = GL_NONE;
    switch(lerpType)
    {
        case TexLerpType::Linear: {
            glLerpType = GL_LINEAR;
            break;
        }
        case TexLerpType::Nearest: {
            glLerpType = GL_NEAREST;
            break;
        }
        default: {
            assert(false && "Invalid texture lerp type");
        }
    }
    return glLerpType;
}

GLenum getGLWrapType(TexWrapType wrapType) {
    GLenum glWrapType = GL_NONE;
    switch(wrapType) {
        case TexWrapType::Repeat: {
            glWrapType = GL_REPEAT;
            break;
        }
        case TexWrapType::MirroredRepeat: {
            glWrapType = GL_MIRRORED_REPEAT;
            break;
        }
        case TexWrapType::ClamToEdge: {
            glWrapType = GL_CLAMP_TO_EDGE;
            break;
        }
        default: {
            assert(false && "Invalid texture wrap type");
        }
    }
    return glWrapType;
}

} // namespace

RenderTexture::RenderTexture() :
    texId(0),
    size(0) {
}

RenderTexture::~RenderTexture() {
}

void RenderTexture::modifyRGBA(const Vec2i& startPt, const Vec2i& endPt, const ColorB* pixels) {
    Vec2i size = endPt - startPt;
    glBindTexture(GL_TEXTURE_2D, texId);
    {
        glTexSubImage2D(GL_TEXTURE_2D, 0, startPt.x, startPt.y, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderTexture::setTexLerpType(TexLerpType minType, TexLerpType magType) {
    glBindTexture(GL_TEXTURE_2D, texId);
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getGLFilterType(minType));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getGLFilterType(magType));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderTexture::setWrapType(TexWrapType sWrapType, TexWrapType tWrapType) {
    glBindTexture(GL_TEXTURE_2D, texId);
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getGLWrapType(sWrapType));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getGLWrapType(tWrapType));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}