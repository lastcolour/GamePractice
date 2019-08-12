#ifndef __RENDER_TEXTURE_MANAGER_HPP__
#define __RENDER_TEXTURE_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Render/ETRenderInterfaces.hpp"

class Buffer;

class RenderTextureManager : public SystemLogic,
    public ETNode<ETRenderTextureManger> {
public:

    RenderTextureManager();
    virtual ~RenderTextureManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRenderTextureManger
    std::shared_ptr<RenderTexture> ET_createTexture(const char* textureName) override;

private:

    std::shared_ptr<RenderTexture> createTexture(const Buffer& buffer);

private:

    std::unordered_map<std::string, std::weak_ptr<RenderTexture>> textures;
};

#endif /* __RENDER_TEXTURE_MANAGER_HPP__ */