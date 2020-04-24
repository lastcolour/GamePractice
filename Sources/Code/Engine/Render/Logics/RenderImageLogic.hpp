#ifndef __RENDER_IMAGE_LOGIC_HPP__
#define __RENDER_IMAGE_LOGIC_HPP__

#include "Render/Logics/RenderNode.hpp"

class RenderImageLogic : public RenderNode,
    public ETNode<ETRenderImageLogic>,
    public ETNode<ETRenderRect> {
public:

    RenderImageLogic();
    virtual ~RenderImageLogic();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETRenderEvents
    void ET_onRender(const RenderContext& renderCtx) override;

    // ETRenderImageLogic
    void ET_setImage(const char* imageName) override;
    void ET_setScale(const Vec2& newScale) override;
    Vec2i ET_getOriginalSize() const override;

    // ETRenderRect
    void ET_setSize(const Vec2i& newSize) override;
    Vec2i ET_getSize() const override;

    // RenderNode
    bool ET_getScrMinusAlphaBlendFlag() const override;

protected:

    void updateScale();

protected:

    std::shared_ptr<RenderTexture> tex;
    Vec2 imageScale;
    Vec2 texScale;
};

#endif /* __RENDER_IMAGE_LOGIC_HPP__ */