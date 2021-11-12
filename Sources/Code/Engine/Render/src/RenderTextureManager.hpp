#ifndef __RENDER_TEXTURE_MANAGER_HPP__
#define __RENDER_TEXTURE_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Render/ETRenderManager.hpp"

class RenderTextureManager : public SystemLogic,
    public ETNode<ETRenderTextureManager> {
public:

    RenderTextureManager();
    virtual ~RenderTextureManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRenderTextureManager
    std::shared_ptr<RenderTexture> ET_createFromFile(const char* imageName, ETextureDataType type) override;
    std::shared_ptr<RenderTexture> ET_createTexture(ETextureDataType type) override;
    std::shared_ptr<RenderFramebuffer> ET_createFramebuffer(EFramebufferType type) override;

private:

    std::shared_ptr<RenderTexture> createTexture(const Memory::Buffer& buffer, ETextureDataType texType);

private:

    std::unordered_map<std::string, std::shared_ptr<RenderTexture>> fileTextures;
    std::vector<std::shared_ptr<RenderTexture>> textures;
    std::vector<std::shared_ptr<RenderFramebuffer>> framebuffers;
};

#endif /* __RENDER_TEXTURE_MANAGER_HPP__ */