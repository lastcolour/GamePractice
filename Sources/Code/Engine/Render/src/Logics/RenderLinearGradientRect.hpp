#ifndef __RENDER_LINEAR_GRADIENT_RECT_HPP__
#define __RENDER_LINEAR_GRADIENT_RECT_HPP__

#include "Logics/RenderNode.hpp"

class ReflectContext;

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
    void ET_setSize(const Vec2& newSize) override;
    Vec2 ET_getSize() const override;

private:

    Vec2 size;
    ColorB startCol;
    ColorB endCol;
    bool isVertical;
};

#endif /* __RENDER_LINEAR_GRADIENT_RECT_HPP__ */