#ifndef __RENDER_IMAGE_LOGIC_HPP__
#define __RENDER_IMAGE_LOGIC_HPP__

#include "Logics/DrawCommandProxy.hpp"

class RenderImageLogic : public DrawCommandProxy,
    public ETNode<ETRenderImageLogic>,
    public ETNode<ETRenderRect> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderImageLogic();
    virtual ~RenderImageLogic();

    // ETRenderImageLogic
    TextureInfo ET_getTextureInfo() const override;
    void ET_setTextureInfo(const TextureInfo& newTexture) override;

    // ETRenderRect
    void ET_setSize(const Vec2& newSize) override;
    Vec2 ET_getSize() const override;

protected:

    // DrawCommandProxy
    void onInit() override;

protected:

    TextureInfo textureInfo;
    Vec2 size;
};

#endif /* __RENDER_IMAGE_LOGIC_HPP__ */