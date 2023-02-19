#include "RenderTexture.hpp"
#include "RenderUtils.hpp"

#include <cassert>

RenderTexture::RenderTexture() :
    size(-1),
    texId(0),
    minLerpType(ETextureLerpType::Unset),
    magLerpType(ETextureLerpType::Unset),
    sWrapType(ETextureWrapType::Unset),
    tWrapType(ETextureWrapType::Unset),
    dataType(ETextureDataType::RGBA),
    bound(false) {
}

RenderTexture::~RenderTexture() {
}

RenderTexture::RenderTexture(RenderTexture&& other) :
    size(other.size),
    texId(other.texId),
    minLerpType(other.minLerpType),
    magLerpType(other.magLerpType),
    sWrapType(other.sWrapType),
    tWrapType(other.tWrapType),
    dataType(other.dataType),
    bound(other.bound) {

    other.size = Vec2i(0);
    other.texId = 0;
    other.minLerpType = ETextureLerpType::Unset;
    other.magLerpType = ETextureLerpType::Unset;
    other.sWrapType = ETextureWrapType::Unset;
    other.tWrapType = ETextureWrapType::Unset;
    other.bound = false;
}

RenderTexture& RenderTexture::operator=(RenderTexture&& other) {
    if(this != &other) {
        this->~RenderTexture();

        size = other.size;
        texId = other.texId;
        minLerpType = other.minLerpType;
        magLerpType = other.magLerpType;
        sWrapType = other.sWrapType;
        tWrapType = other.tWrapType;
        dataType = other.dataType;
        bound = other.bound;

        other.size = Vec2i(0);
        other.texId = 0;
        other.minLerpType = ETextureLerpType::Unset;
        other.magLerpType = ETextureLerpType::Unset;
        other.sWrapType = ETextureWrapType::Unset;
        other.tWrapType = ETextureWrapType::Unset;
        other.bound = false;
    }

    return *this;
}

void RenderTexture::bind() {
    // assert(!bound && "Texture is already bound");

    bound = true;
    glBindTexture(GL_TEXTURE_2D, texId);
}

void RenderTexture::unbind() {
    assert(bound && "Texture isn't bound");

    bound = false;
    glBindTexture(GL_TEXTURE_2D, 0);
}

bool RenderTexture::isBound() const {
    return bound;
}

void RenderTexture::writeR8(const Vec2i& pt, const Vec2i& subSize, const void* data) {
    assert(bound && "Texture isn't bound");

    glTexSubImage2D(GL_TEXTURE_2D, 0, pt.x, pt.y, subSize.x, subSize.y, GL_RED, GL_UNSIGNED_BYTE, data);
}

void RenderTexture::writeRGB(const Vec2i& pt, const Vec2i& subSize, const void* data) {
    assert(bound && "Texture isn't bound");

    glTexSubImage2D(GL_TEXTURE_2D, 0, pt.x, pt.y, subSize.x, subSize.y, GL_RGB, GL_UNSIGNED_BYTE, data);
}

void RenderTexture::writeRGBA(const Vec2i& pt, const Vec2i& subSize, const void* data) {
    assert(bound && "Texture isn't bound");

    glTexSubImage2D(GL_TEXTURE_2D, 0, pt.x, pt.y, subSize.x, subSize.y, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void RenderTexture::setLerpType(ETextureLerpType minType, ETextureLerpType magType) {
    assert(bound && "Texture isn't bound");
    assert(minType != ETextureLerpType::Unset && magType != ETextureLerpType::Unset && "Invalid lerp type");

    if(minLerpType != minType) {
        minLerpType = minType;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, RenderUtils::GetGLTexLerpType(minType));
    }
    if(magLerpType != magType) {
        magLerpType = magType;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, RenderUtils::GetGLTexLerpType(magType));
    }
}

void RenderTexture::setWrapType(ETextureWrapType sWrap, ETextureWrapType tWrap) {
    assert(bound && "Texture isn't bound");

    assert(sWrap != ETextureWrapType::Unset && tWrap != ETextureWrapType::Unset && "Invalid wrap type");
    if(sWrapType != sWrap) {
        sWrapType = sWrap;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, RenderUtils::GetGLTexWrapType(sWrapType));
    }
    if(tWrapType != tWrap) {
        tWrapType = tWrap;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, RenderUtils::GetGLTexWrapType(tWrapType));
    }
}

const Vec2i& RenderTexture::getSize() const {
    return size;
}

bool RenderTexture::resize(const Vec2i& newSize) {
    assert(bound && "Texture isn't bound");

    if(size.x == newSize.x && size.y == newSize.y) {
        return true;
    }
    if(newSize.x <= 0 || newSize.y <= 0) {
        LogError("[ RenderTexture::resize] Invalid new size: [%d x %d]", newSize.x, newSize.y);
        return false;
    }
    int mipLvl = 0;
    switch(dataType) {
        case ETextureDataType::R8: {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, mipLvl, GL_R8, newSize.x, newSize.y, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
            break;
        }
        case ETextureDataType::RGB: {
            glTexImage2D(GL_TEXTURE_2D, mipLvl, GL_RGB, newSize.x, newSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            break;
        }
        case ETextureDataType::RGBA: {
            glTexImage2D(GL_TEXTURE_2D, mipLvl, GL_RGBA, newSize.x, newSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
            break;
        }
        default: {
            assert(false && "Invalid texture type");
        }
    }
    if(auto errStr = RenderUtils::GetGLError()) {
        LogError("[RenderTexture::resize] Can't peform resize [%d x %d] -> [%d x %d] (Error: %s)",
            size.x, size.y, newSize.x, newSize.y, errStr);
        return false;
    }
    size = newSize;
    return true;
}

bool RenderTexture::clear() {
    assert(bound && "Texture isn't bound");

    int mipLvl = 0;
    switch(dataType) {
        case ETextureDataType::R8: {
            Memory::Buffer buff(size.x * size.y);
            memset(buff.getWriteData(), 0, buff.getSize());
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexSubImage2D(GL_TEXTURE_2D, mipLvl, 0, 0, size.x, size.y, GL_RED, GL_UNSIGNED_BYTE, buff.getReadData());
            break;
        }
        case ETextureDataType::RGB: {
            Memory::Buffer buff(size.x * size.y * 3);
            memset(buff.getWriteData(), 0, buff.getSize());
            glTexSubImage2D(GL_TEXTURE_2D, mipLvl, 0, 0, size.x, size.y, GL_RGB, GL_UNSIGNED_BYTE, buff.getReadData());
            break;
        }
        case ETextureDataType::RGBA: {
            Memory::Buffer buff(size.x * size.y * 4);
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

void RenderTexture::setTexInfo(const TextureInfo& texInfo) {
    assert(bound && "Texture isn't bound");

    setWrapType(texInfo.wrapType, texInfo.wrapType);
    setLerpType(texInfo.lerpType, texInfo.lerpType);
}