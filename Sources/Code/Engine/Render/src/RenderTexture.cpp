#include "RenderTexture.hpp"
#include "Platform/OpenGL.hpp"
#include "RenderUtils.hpp"

#include <cassert>

namespace {

GLenum getGLFilterType(ETextureLerpType lerpType) {
    GLenum glLerpType = GL_NONE;
    switch(lerpType)
    {
        case ETextureLerpType::Linear: {
            glLerpType = GL_LINEAR;
            break;
        }
        case ETextureLerpType::Point: {
            glLerpType = GL_NEAREST;
            break;
        }
        default: {
            assert(false && "Invalid texture lerp type");
        }
    }
    return glLerpType;
}

GLenum getGLWrapType(ETextureWrapType wrapType) {
    GLenum glWrapType = GL_NONE;
    switch(wrapType) {
        case ETextureWrapType::Repeat: {
            glWrapType = GL_REPEAT;
            break;
        }
        case ETextureWrapType::MirroredRepeat: {
            glWrapType = GL_MIRRORED_REPEAT;
            break;
        }
        case ETextureWrapType::ClamToEdge: {
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
    size(-1),
    texId(0),
    minLerpType(ETextureLerpType::Point),
    magLerpType(ETextureLerpType::Point),
    sWrapType(ETextureWrapType::ClamToEdge),
    tWrapType(ETextureWrapType::ClamToEdge),
    dataType(ETextureDataType::RGBA) {
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

void RenderTexture::setLerpType(ETextureLerpType minType, ETextureLerpType magType) {
    if(minLerpType != minType) {
        minLerpType = minType;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getGLFilterType(minType));

    }
    if(magLerpType != magType) {
        magLerpType = magType;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getGLFilterType(magType));

    }
}

void RenderTexture::setWrapType(ETextureWrapType sWrap, ETextureWrapType tWrap) {
    if(sWrapType != sWrap) {
        sWrapType = sWrap;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getGLWrapType(sWrapType));
    }
    if(tWrapType != tWrap) {
        tWrapType = tWrap;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getGLWrapType(tWrapType));
    }
}

const Vec2i& RenderTexture::getSize() const {
    return size;
}

bool RenderTexture::resize(const Vec2i& newSize) {
    size = newSize;
    int mipLvl = 0;
    switch(dataType) {
        case ETextureDataType::R8: {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, mipLvl, GL_R8, size.x, size.y, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
            break;
        }
        case ETextureDataType::RGB: {
            glTexImage2D(GL_TEXTURE_2D, mipLvl, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            break;
        }
        case ETextureDataType::RGBA: {
            glTexImage2D(GL_TEXTURE_2D, mipLvl, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
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

bool RenderTexture::clear() {
    int mipLvl = 0;
    switch(dataType) {
        case ETextureDataType::R8: {
            Buffer buff(size.x * size.y);
            memset(buff.getWriteData(), 0, buff.getSize());
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexSubImage2D(GL_TEXTURE_2D, mipLvl, 0, 0, size.x, size.y, GL_RED, GL_UNSIGNED_BYTE, buff.getReadData());
            break;
        }
        case ETextureDataType::RGB: {
            Buffer buff(size.x * size.y * 3);
            memset(buff.getWriteData(), 0, buff.getSize());
            glTexSubImage2D(GL_TEXTURE_2D, mipLvl, 0, 0, size.x, size.y, GL_RGB, GL_UNSIGNED_BYTE, buff.getReadData());
            break;
        }
        case ETextureDataType::RGBA: {
            Buffer buff(size.x * size.y * 4);
            memset(buff.getWriteData(), 0, buff.getSize());
            glTexSubImage2D(GL_TEXTURE_2D, mipLvl, 0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, buff.getReadData());
            break;
        }
        default: {
            assert(false && "Invalid texture type");
        }
    }
    if(auto errStr = RenderUtils::GetGLError()) {
        LogError("[RenderTexture::clear] Can't clear texture (Error: %s)", errStr);
        return false;
    }
    return true;
}