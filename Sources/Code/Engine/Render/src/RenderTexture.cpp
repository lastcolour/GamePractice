#include "RenderTexture.hpp"
#include "Platform/OpenGL.hpp"
#include "RenderUtils.hpp"
#include "Core/ETLogger.hpp"
#include "Core/Buffer.hpp"

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

void RenderTexture::bind() const {
    glBindTexture(GL_TEXTURE_2D, texId);
}

void RenderTexture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderTexture::writeR8(const Vec2i& pt, const Vec2i& subSize, const void* data) {
    glTexSubImage2D(GL_TEXTURE_2D, 0, pt.x, pt.y, subSize.x, subSize.y, GL_RED, GL_UNSIGNED_BYTE, data);
}

void RenderTexture::writeRGB(const Vec2i& pt, const Vec2i& subSize, const void* data) {
    glTexSubImage2D(GL_TEXTURE_2D, 0, pt.x, pt.y, subSize.x, subSize.y, GL_RGB, GL_UNSIGNED_BYTE, data);
}

void RenderTexture::writeRGBA(const Vec2i& pt, const Vec2i& subSize, const void* data) {
    glTexSubImage2D(GL_TEXTURE_2D, 0, pt.x, pt.y, subSize.x, subSize.y, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void RenderTexture::setPixelLerpType(TexLerpType minType, TexLerpType magType) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getGLFilterType(minType));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getGLFilterType(magType));
}

void RenderTexture::setPixelWrapType(TexWrapType sWrapType, TexWrapType tWrapType) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getGLWrapType(sWrapType));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getGLWrapType(tWrapType));
}

Vec2i RenderTexture::getSize() const {
    return size;
}

bool RenderTexture::resize(const Vec2i& newSize) {
    size = newSize;
    switch(type) {
        case ETextureType::R8: {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, size.x, size.y, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
            break;
        }
        case ETextureType::RGB: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            break;
        }
        case ETextureType::RGBA: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            break;
        }
        default: {
            assert(false && "Invalid texture type");
        }
    }
    if(auto errStr = RenderUtils::GetGLError()) {
        LogError("[RenderTexture::resize] Can't resize texture to a size %dx%d (Error: %s)", newSize.x, newSize.y, errStr);
        return false;
    }
    return true;
}

bool RenderTexture::resizeAndClear(const Vec2i& newSize) {
    size = newSize;
    switch(type) {
        case ETextureType::R8: {
            Buffer buff(size.x * size.y);
            memset(buff.getWriteData(), 0, buff.getSize());
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, size.x, size.y, 0, GL_RED, GL_UNSIGNED_BYTE, buff.getReadData());
            break;
        }
        case ETextureType::RGB: {
            Buffer buff(size.x * size.y * 3);
            memset(buff.getWriteData(), 0, buff.getSize());
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, buff.getReadData());
            break;
        }
        case ETextureType::RGBA: {
            Buffer buff(size.x * size.y * 4);
            memset(buff.getWriteData(), 0, buff.getSize());
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, buff.getReadData());
            break;
        }
        default: {
            assert(false && "Invalid texture type");
        }
    }
    if(auto errStr = RenderUtils::GetGLError()) {
        LogError("[RenderTexture::resizeAndClear] Can't resize texture to a size %dx%d (Error: %s)", errStr);
        return false;
    }
    return true;
}