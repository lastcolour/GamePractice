#ifndef __RENDER_LINEAR_GRADIENT_RECT_HPP__
#define __RENDER_LINEAR_GRADIENT_RECT_HPP__

#include "Entity/EntityLogic.hpp"
#include "Render/Logics/RenderNode.hpp"
#include "ETApplicationInterfaces.hpp"

class RenderLinearGradientRect : public RenderNode,
    public ETNode<ETRenderRect> {
public:

    RenderLinearGradientRect();
    virtual ~RenderLinearGradientRect();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // RenderNode
    void ET_onRender(const RenderContext& renderCtx) override;

    // ETRenderRect
    void ET_setSize(const Vec2i& newSize) override;
    Vec2i ET_getSize() const override;

private:

    void updateTexData();

private:

    std::shared_ptr<RenderTexture> tex;
    Vec2 scale;
    ColorB startCol;
    ColorB endCol;
    bool isVertical;
};

#endif /* __RENDER_LINEAR_GRADIENT_RECT_HPP__ */