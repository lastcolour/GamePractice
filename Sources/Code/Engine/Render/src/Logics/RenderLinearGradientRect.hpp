#ifndef __RENDER_LINEAR_GRADIENT_RECT_HPP__
#define __RENDER_LINEAR_GRADIENT_RECT_HPP__

#include "Logics/RenderImageLogic.hpp"

class RenderLinearGradientRect : public DrawCommandProxy,
    public ETNode<ETRenderRect> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderLinearGradientRect();
    virtual ~RenderLinearGradientRect();

    // ETRenderRect
    void ET_setSize(const Vec2& newSize) override;
    Vec2 ET_getSize() const override;

protected:

    // DrawCommandProxy
    void onInit() override;

private:

    Vec2 size;
    ColorB startCol;
    ColorB endCol;
    bool isVertical;
};

#endif /* __RENDER_LINEAR_GRADIENT_RECT_HPP__ */