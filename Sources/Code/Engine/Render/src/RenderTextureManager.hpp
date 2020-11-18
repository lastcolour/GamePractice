#ifndef __RENDER_TEXTURE_MANAGER_HPP__
#define __RENDER_TEXTURE_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Render/ETRenderManager.hpp"

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
    std::shared_ptr<RenderTexture> ET_createFromImage(const char* imageName, ETextureType type) override;
    std::shared_ptr<RenderTexture> ET_createTexture(ETextureType type) override;
    std::shared_ptr<RenderFramebuffer> ET_createFramebuffer() override;

private:

    std::shared_ptr<RenderTexture> createTexture(const Buffer& buffer, ETextureType texType);

private:

    std::unordered_map<std::string, std::shared_ptr<RenderTexture>> images;
    std::vector<std::shared_ptr<RenderTexture>> textures;
    std::vector<std::shared_ptr<RenderFramebuffer>> framebuffers;
};

#endif /* __RENDER_TEXTURE_MANAGER_HPP__ */