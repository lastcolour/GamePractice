#ifndef __RENDER_IMAGE_LOGIC_HPP__
#define __RENDER_IMAGE_LOGIC_HPP__

#include "Logics/RenderNode.hpp"

class RenderTexture;
class ReflectContext;

class RenderImageLogic : public RenderNode,
    public ETNode<ETRenderImageLogic>,
    public ETNode<ETRenderRect> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderImageLogic();
    virtual ~RenderImageLogic();

    // EntityLogic
    bool init() override;

    // ETRenderImageLogic
    void ET_setImage(const char* imageName) override;
    Vec2i ET_getImageSize() const override;

    // ETRenderRect
    void ET_setSize(const Vec2i& newSize) override;
    Vec2i ET_getSize() const override;

    // RenderNode
    bool ET_isVisible() const override;

protected:

    // RenderNode
    void onRender(RenderContext& renderCtx) override;

protected:

    std::shared_ptr<RenderTexture> tex;
    Vec2i size;
};

#endif /* __RENDER_IMAGE_LOGIC_HPP__ */