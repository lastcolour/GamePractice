#ifndef __RENDER_SIMPLE_LOGIC_HPP__
#define __RENDER_SIMPLE_LOGIC_HPP__

#include "Logics/DrawCommandProxy.hpp"

class RenderSimpleLogic : public DrawCommandProxy,
    public ETNode<ETRenderSimpleLogic>,
    public ETNode<ETRenderRect> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderSimpleLogic();
    virtual ~RenderSimpleLogic();

    // ETRenderSimpleLogic
    void ET_setColor(const ColorB& newColor) override;

    // ETRenderRect
    void ET_setSize(const Vec2& newSize) override;
    Vec2 ET_getSize() const override;

protected:

    // DrawCommandProxy
    void onInit() override;
    Mat4 calcModelMat() const override;

private:

    Vec2 size;
    ColorB color;
};

#endif /* __RENDER_SIMPLE_LOGIC_HPP__ */