#include "Render/RenderTextureManager.hpp"
#include "Render/RenderTexture.hpp"
#include "Platforms/OpenGL.hpp"
#include "Core/Buffer.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Render/RenderUtils.hpp"

#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#include <stb_image.h>

RenderTextureManager::RenderTextureManager() {
}

RenderTextureManager::~RenderTextureManager() {
}

bool RenderTextureManager::init() {
    ETNode<ETRenderTextureManger>::connect(getEntityId());
    ETNode<ETRenderResourceManager>::connect(getEntityId());
    return true;
}

void RenderTextureManager::deinit() {
    ETNode<ETRenderTextureManger>::disconnect();
    ETNode<ETRenderResourceManager>::disconnect();
}

std::string RenderTextureManager::createNewTexSizeName(const Vec2i& texSize) const {
    std::string texNamePreffix = StringFormat("[%dx%d]_", texSize.x, texSize.y);
    int tex_number = 0;
    for(auto& texNode : textures) {
        const auto& texName = texNode.first;
        auto findPt = texName.find(texNamePreffix);
        if(findPt != std::string::npos) {
            std::string texNumStr = texName.substr(findPt + texNamePreffix.length());
            int usedTexNum = std::stoi(texNumStr);
            if(usedTexNum > tex_number) {
                tex_number = usedTexNum + 1;
            }
        }
    }
    return StringFormat("%s%d", texNamePreffix, tex_number);
}

std::string RenderTextureManager::getLookupName(const char* textureName, ETextureType texType) const {
    const char* suffix = "";
    switch(texType) {
        case ETextureType::SingleColor: {
            suffix = "_single";
            break;
        }
        case ETextureType::NormalColor: {
            suffix = "_normal";
            break;
        }
        default: {
            assert(false && "Invalid texture type");
        }
    }
    return StringFormat("%s%s", textureName, suffix);
}

std::shared_ptr<RenderTexture> RenderTextureManager::ET_createTexture(const char* textureName, ETextureType texType) {
    std::string texLookupName = getLookupName(textureName, texType);
    auto it = textures.find(texLookupName);
    if(it != textures.end() && !it->second) {
        return it->second;
    }

    Buffer buff;
    ET_SendEventReturn(buff, &ETAssets::ET_loadAsset, textureName);
    if(!buff) {
        LogWarning("[RenderTextureManager::ET_createTexture] Can't load texture from: '%s'", textureName);
        return nullptr;
    }

    auto tex = createTexture(buff, texType);
    if(!tex) {
        return nullptr;
    }
    textures[texLookupName] = tex;
    return tex;
}

std::shared_ptr<RenderTexture> RenderTextureManager::ET_createEmptyTexture(const Vec2i& texSize, ETextureType texType) {
    auto tex = createEmptyTexture(texSize, texType);
    if(!tex) {
        return nullptr;
    }

    auto texLookupName = createNewTexSizeName(texSize);
    textures[texLookupName] = tex;
    return tex;
}

std::shared_ptr<RenderTexture> RenderTextureManager::createTexture(const Buffer& buff, ETextureType texType) {
    unsigned int textureId = 0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int reqLoadChannel = 0;
    switch(texType) {
        case ETextureType::SingleColor: {
            reqLoadChannel = 1;
            break;
        }
        case ETextureType::NormalColor: {
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
        LogError("[RenderTextureManager::createTexture] Can't create texture from buffer");
        glBindTexture(GL_TEXTURE_2D, 0);
        glDeleteTextures(1, &textureId);
        return nullptr;
    }

    switch(texType) {
        case ETextureType::SingleColor: {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, texSize.x, texSize.y, 0, GL_RED, GL_UNSIGNED_BYTE, static_cast<const GLvoid*>(data));
            break;
        }
        case ETextureType::NormalColor: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texSize.x, texSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<const GLvoid*>(data));
            break;
        }
        default: {
            assert(false && "Invalid texture type");
            return nullptr;
        }
    }

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

std::shared_ptr<RenderTexture> RenderTextureManager::createEmptyTexture(const Vec2i& texSize, ETextureType texType) {
    if(texType != ETextureType::SingleColor) {
        assert(false && "Unsupported texture format");
        LogError("[RenderTextureManager::createEmptyTexture] Can't create empty texture of type: %d", texType);
        return nullptr;
    }

    unsigned int textureId = 0;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Buffer buff(texSize.x * texSize.y * 2);
    memset(buff.getWriteData(), 0, buff.getSize());

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, texSize.x, texSize.y, 0, GL_RED, GL_UNSIGNED_BYTE, buff.getReadData());

    if(!CheckGLError()) {
        LogError("[RenderTextureManager::createEmptyTexture] Can't transfter texture data from app to OpenGL");
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

void RenderTextureManager::ET_forgetResoruces() {
    textures.clear();
}

void RenderTextureManager::ET_cleanUnused() {
    auto it = textures.begin();
    while(it != textures.end()) {
        auto& texPtr = it->second;
        if(texPtr.use_count() == 1) {
            glDeleteTextures(1, &(texPtr->texId));
            it = textures.erase(it);
        } else {
            ++it;
        }
    }
}
