#include "RenderTextureManager.hpp"
#include "RenderTexture.hpp"
#include "RenderFramebuffer.hpp"
#include "RenderUtils.hpp"
#include "Core/ETAssets.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include <stb_image.h>

namespace {

std::string getLookupName(const char* imageName, ETextureDataType texType) {
    const char* suffix = "";
    switch(texType) {
        case ETextureDataType::R8: {
            suffix = "_r8";
            break;
        }
        case ETextureDataType::RGB: {
            suffix = "_rgb";
            break;
        }
        case ETextureDataType::RGBA: {
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
    ETNode<ETRenderTextureManager>::connect(getEntityId());
    return true;
}

void RenderTextureManager::deinit() {
    ETNode<ETRenderTextureManager>::disconnect();
}

std::shared_ptr<RenderTexture> RenderTextureManager::ET_createFromFile(const char* fileName, ETextureDataType texType) {
    assert(RenderUtils::IsOpenGLContextExists() && "Can't create image without OpenGL context");

    std::string reqImageName = getLookupName(fileName, texType);
    auto it = fileTextures.find(reqImageName);
    if(it != fileTextures.end() && it->second) {
        return it->second;
    }

    Memory::Buffer buff;
    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, fileName);
    if(!buff) {
        LogWarning("[RenderTextureManager::ET_createTexture] Can't load image from: '%s'", fileName);
        return nullptr;
    }

    auto tex = createTexture(buff, texType);
    if(!tex) {
        return nullptr;
    }
    fileTextures[reqImageName] = tex;
    return tex;
}

std::shared_ptr<RenderTexture> RenderTextureManager::ET_createTexture(ETextureDataType texType) {
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
    texture->dataType = texType;
    texture->texId = textureId;
    texture->size = Vec2i(0);
    texture->setWrapType(ETextureWrapType::Repeat, ETextureWrapType::Repeat);
    texture->setLerpType(ETextureLerpType::Linear, ETextureLerpType::Linear);
    texture->unbind();

    textures.push_back(texture);

    return texture;
}

std::shared_ptr<RenderTexture> RenderTextureManager::createTexture(const Memory::Buffer& buff, ETextureDataType texType) {
    unsigned int textureId = 0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    if(auto errStr = RenderUtils::GetGLError()) {
        LogError("[RenderTextureManager::createEmptyTexture] Can't create texture (Error: %s)", errStr);
        return nullptr;
    }

    int reqLoadChannel = 0;
    switch(texType) {
        case ETextureDataType::R8: {
            reqLoadChannel = 1;
            break;
        }
        case ETextureDataType::RGB: {
            reqLoadChannel = 3;
            break;
        }
        case ETextureDataType::RGBA: {
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
        case ETextureDataType::R8: {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, texSize.x, texSize.y, 0, GL_RED, GL_UNSIGNED_BYTE, static_cast<const GLvoid*>(data));
            break;
        }
        case ETextureDataType::RGB: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texSize.x, texSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE, static_cast<const GLvoid*>(data));
            break;
        }
        case ETextureDataType::RGBA: {
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
    texture->dataType = texType;
    texture->texId = textureId;
    texture->size = texSize;
    texture->setWrapType(ETextureWrapType::Repeat, ETextureWrapType::Repeat);
    texture->setLerpType(ETextureLerpType::Linear, ETextureLerpType::Linear);
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
    framebuffer->color0.dataType = ETextureDataType::RGB;
    framebuffer->color0.texId = textureId;
    framebuffer->color0.size = Vec2i(0);
    framebuffer->color0.setWrapType(ETextureWrapType::Repeat, ETextureWrapType::Repeat);
    framebuffer->color0.setLerpType(ETextureLerpType::Linear, ETextureLerpType::Linear);
    framebuffer->renderBufferId = renderBufferId;
    framebuffer->type = type;
    framebuffer->unbind();

    framebuffers.push_back(framebuffer);

    return framebuffer;
}