#ifndef __RENDER_TEXTURE_MANAGER_HPP__
#define __RENDER_TEXTURE_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Render/ETRenderManager.hpp"

class Buffer;

class RenderTextureManager : public SystemLogic,
    public ETNode<ETRenderTextureManger>,
    public ETNode<ETRenderResourceManager> {
public:

    RenderTextureManager();
    virtual ~RenderTextureManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRenderTextureManger
    std::shared_ptr<RenderTexture> ET_createTexture(const char* textureName, ETextureType texType) override;
    std::shared_ptr<RenderTexture> ET_createEmptyTexture(const Vec2i& texSize, ETextureType texType) override;

    // ETRenderResourceManager
    void ET_forgetResoruces() override;
    void ET_cleanUnused() override;

private:

    std::string createNewTexSizeName(const Vec2i& texSize) const;
    std::string getLookupName(const char* textureName, ETextureType texType) const;

    std::shared_ptr<RenderTexture> createEmptyTexture(const Vec2i& texSize, ETextureType texType);
    std::shared_ptr<RenderTexture> createTexture(const Buffer& buffer, ETextureType texType);

private:

    std::unordered_map<std::string, std::shared_ptr<RenderTexture>> textures;
};

#endif /* __RENDER_TEXTURE_MANAGER_HPP__ */