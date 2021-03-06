#include "RenderTextureManager.hpp"
#include "RenderTexture.hpp"
#include "RenderFramebuffer.hpp"
#include "RenderUtils.hpp"
#include "Platform/OpenGL.hpp"
#include "Core/Buffer.hpp"
#include "Core/ETLogger.hpp"
#include "Core/ETAssets.hpp"

#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include <stb_image.h>

namespace {

std::string getLookupName(const char* imageName, ETextureType texType) {
    const char* suffix = "";
    switch(texType) {
        case ETextureType::R8: {
            suffix = "_r8";
            break;
        }
        case ETextureType::RGB: {
            suffix = "_rgb";
            break;
        }
        case ETextureType::RGBA: {
            suffix = "_rgba";
            break;
        }
        default: {
            assert(false && "Invalid texture type");
        }
    }
    return StringFormat("%s%s", imageName, suffix);
}

} // namespace

RenderTextureManager::RenderTextureManager() {
}

RenderTextureManager::~RenderTextureManager() {
}

bool RenderTextureManager::init() {
    ETNode<ETRenderTextureManger>::connect(getEntityId());
    return true;
}

void RenderTextureManager::deinit() {
    ETNode<ETRenderTextureManger>::disconnect();
}

std::shared_ptr<RenderTexture> RenderTextureManager::ET_createFromImage(const char* imageName, ETextureType texType) {
    assert(RenderUtils::IsOpenGLContextExists() && "Can't create image without OpenGL context");

    std::string reqImageName = getLookupName(imageName, texType);
    auto it = images.find(reqImageName);
    if(it != images.end() && it->second) {
        return it->second;
    }

    Buffer buff;
    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, imageName);
    if(!buff) {
        LogWarning("[RenderTextureManager::ET_createTexture] Can't load image from: '%s'", imageName);
        return nullptr;
    }

    auto tex = createTexture(buff, texType);
    if(!tex) {
        return nullptr;
    }
    images[reqImageName] = tex;
    return tex;
}

std::shared_ptr<RenderTexture> RenderTextureManager::ET_createTexture(ETextureType texType) {
    assert(RenderUtils::IsOpenGLContextExists() && "Can't create textrure without OpenGL context");

    unsigned int textureId = 0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    if(textureId == 0) {
        LogError("[RenderTextureManager::ET_createTexture] Can't create basic texture");
        assert(false && "Invalid OpenGL context state");
        return nullptr;
    }

    if(auto errStr = RenderUtils::GetGLError()) {
        LogError("[RenderTextureManager::createEmptyTexture] Can't create texture (Error: %s)", errStr);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &textureId);
        return nullptr;
    }

    std::shared_ptr<RenderTexture> texture(new RenderTexture);
    texture->type = texType;
    texture->texId = textureId;
    texture->size = Vec2i(0);
    texture->setPixelWrapType(TexWrapType::Repeat, TexWrapType::Repeat);
    texture->setPixelLerpType(TexLerpType::Nearest, TexLerpType::Nearest);
    texture->unbind();

    textures.push_back(texture);

    return texture;
}

std::shared_ptr<RenderTexture> RenderTextureManager::createTexture(const Buffer& buff, ETextureType texType) {
    unsigned int textureId = 0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    if(auto errStr = RenderUtils::GetGLError()) {
        LogError("[RenderTextureManager::createEmptyTexture] Can't create texture (Error: %s)", errStr);
        return nullptr;
    }

    int reqLoadChannel = 0;
    switch(texType) {
        case ETextureType::R8: {
            reqLoadChannel = 1;
            break;
        }
        case ETextureType::RGB: {
            reqLoadChannel = 3;
            break;
        }
        case ETextureType::RGBA: {
            reqLoadChannel = 4;
            break;
        }
        default: {
            assert(false && "Invalid texture type");
            return nullptr;
        }
    }

    Vec2i texSize(0);
    int numChannels = 0;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load_from_memory(static_cast<const unsigned char*>(buff.getReadData()),
        static_cast<int>(buff.getSize()), &texSize.x, &texSize.y, &numChannels, reqLoadChannel);

    if(data == nullptr) {
        LogError("[RenderTextureManager::createTexture] Can't parse image buffer");
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &textureId);
        return nullptr;
    }

    switch(texType) {
        case ETextureType::R8: {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, texSize.x, texSize.y, 0, GL_RED, GL_UNSIGNED_BYTE, static_cast<const GLvoid*>(data));
            break;
        }
        case ETextureType::RGB: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texSize.x, texSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, static_cast<const GLvoid*>(data));
            break;
        }
        case ETextureType::RGBA: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize.x, texSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<const GLvoid*>(data));
            break;
        }
        default: {
            assert(false && "Invalid texture type");
            return nullptr;
        }
    }

    stbi_image_free(static_cast<void*>(data));

    if(auto errStr = RenderUtils::GetGLError()) {
        LogError("[RenderTextureManager::createEmptyTexture] Can't copy texture data to GPU (Error: %s)", errStr);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &textureId);
        return nullptr;
    }

    std::shared_ptr<RenderTexture> texture(new RenderTexture);
    texture->type = texType;
    texture->texId = textureId;
    texture->size = texSize;
    texture->setPixelWrapType(TexWrapType::Repeat, TexWrapType::Repeat);
    texture->setPixelLerpType(TexLerpType::Nearest, TexLerpType::Nearest);
    texture->unbind();

    return texture;
}

std::shared_ptr<RenderFramebuffer> RenderTextureManager::ET_createFramebuffer(EFramebufferType type) {
    assert(RenderUtils::IsOpenGLContextExists() && "Can't create framebuffer without OpenGL context");

    unsigned int framebufferId = 0;
    glGenFramebuffers(1, &framebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
    if(auto errStr = RenderUtils::GetGLError()) {
        LogError("[RenderTextureManager::ET_createFramebuffer] Can't generate framebuffer (Error: %s)", errStr);
        return nullptr;
    }

    unsigned int textureId = 0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    if(auto errStr = RenderUtils::GetGLError()) {
        LogError("[RenderTextureManager::ET_createFramebuffer] Can't generate texture for a framebuffer (Error: %s)", errStr);
        glDeleteFramebuffers(1, &framebufferId);
        return nullptr;
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

    if(auto errStr = RenderUtils::GetGLError()) {
        LogError("[RenderTextureManager::ET_createFramebuffer] Can't attach texture a framebuffer (Error: %s)", errStr);
        glDeleteFramebuffers(1, &framebufferId);
        glDeleteTextures(1, &textureId);
        return nullptr;
    }

    unsigned int renderBufferId = 0;
    if(type == EFramebufferType::Color_Depth_Stencil) {
        glGenRenderbuffers(1, &renderBufferId);
        glBindRenderbuffer(GL_RENDERBUFFER, renderBufferId);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 0, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBufferId);

        if(auto errStr = RenderUtils::GetGLError()) {
            LogError("[RenderTextureManager::ET_createFramebuffer] Can't attach stencil buffer (Error: %s)", errStr);
            glDeleteFramebuffers(1, &framebufferId);
            glDeleteTextures(1, &textureId);
            glDeleteRenderbuffers(1, &renderBufferId);
            return nullptr;
        }
    }

    std::shared_ptr<RenderFramebuffer> framebuffer(new RenderFramebuffer);
    framebuffer->framebufferId = framebufferId;
    framebuffer->color0.type = ETextureType::RGB;
    framebuffer->color0.texId = textureId;
    framebuffer->color0.size = Vec2i(0);
    framebuffer->color0.setPixelWrapType(TexWrapType::Repeat, TexWrapType::Repeat);
    framebuffer->color0.setPixelLerpType(TexLerpType::Nearest, TexLerpType::Nearest);
    framebuffer->renderBufferId = renderBufferId;
    framebuffer->type = type;
    framebuffer->unbind();

    framebuffers.push_back(framebuffer);

    return framebuffer;
}