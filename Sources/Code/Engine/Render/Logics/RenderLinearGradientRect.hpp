#ifndef __RENDER_LINEAR_GRADIENT_RECT_HPP__
#define __RENDER_LINEAR_GRADIENT_RECT_HPP__

#include "Entity/EntityLogic.hpp"
#include "Render/Logics/RenderNode.hpp"
#include "ETApplicationInterfaces.hpp"

class RenderLinearGradientRect : public RenderNode,
    public ETNode<ETRenderRect> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderLinearGradientRect();
    virtual ~RenderLinearGradientRect();

    // EntityLogic
    bool init() override;

    // ETRenderRect
    void ET_setSize(const Vec2i& newSize) override;
    Vec2i ET_getSize() const override;

protected:

    // RenderNode
    void onRender(RenderContext& renderCtx) override;

private:

    void updateTexData();

private:

    std::shared_ptr<RenderTexture> tex;
    Vec2i size;
    ColorB startCol;
    ColorB endCol;
    bool isVertical;
};

#endif /* __RENDER_LINEAR_GRADIENT_RECT_HPP__ */