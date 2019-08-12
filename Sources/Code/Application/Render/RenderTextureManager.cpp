#include "Render/RenderTextureManager.hpp"
#include "Render/RenderTexture.hpp"
#include "Platforms/OpenGL.hpp"
#include "Core/Buffer.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Render/RenderUtils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include <stb_image.h>

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

std::shared_ptr<RenderTexture> RenderTextureManager::ET_createTexture(const char* textureName) {
    std::string reqTextureName = textureName;
    auto it = textures.find(reqTextureName);
    if(it != textures.end() && !it->second.expired()) {
        return it->second.lock();
    }

    Buffer buff;
    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, reqTextureName.c_str());
    if(!buff) {
        LogWarning("[RenderTextureManager::ET_createTexture] Can't load texture from: %s", reqTextureName);
        return nullptr;
    }

    auto tex = createTexture(buff);
    if(!tex) {
        return nullptr;
    }
    textures[reqTextureName] = tex;
    return tex;
}

std::shared_ptr<RenderTexture> RenderTextureManager::createTexture(const Buffer& buff) {
    unsigned int textureId = 0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Vec2i texSize(0);
    int numChannels = 0;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load_from_memory(static_cast<const unsigned char*>(buff.getReadData()),
        static_cast<int>(buff.getSize()), &texSize.x, &texSize.y, &numChannels, 4);
    if(data == nullptr) {
        LogError("[RenderTextureManager::createTexture] Can't create texture from buffer");
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &textureId);
        return nullptr;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize.x, texSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<const GLvoid*>(data));
    stbi_image_free(static_cast<void*>(data));
    if(!CheckGLError()) {
        LogError("[RenderTextureManager::createTexture] Can't transfter texture data from app to OpenGL");
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &textureId);
        return nullptr;
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    std::shared_ptr<RenderTexture> texture(new RenderTexture);
    texture->texId = textureId;
    texture->size = texSize;
    return texture;
}
