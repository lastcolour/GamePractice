#ifndef __RENDER_SIMPLE_LOGIC_HPP__
#define __RENDER_SIMPLE_LOGIC_HPP__

#include "Logics/RenderNode.hpp"

class ReflectContext;

class RenderSimpleLogic : public RenderNode,
    public ETNode<ETRenderSimpleLogic>,
    public ETNode<ETRenderRect> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderSimpleLogic();
    virtual ~RenderSimpleLogic();

    // EntityLogic
    void init() override;

    // ETRenderSimpleLogic
    void ET_setColor(const ColorB& col) override;

    // ETRenderRect
    void ET_setSize(const Vec2& newSize) override;
    Vec2 ET_getSize() const override;

protected:

    void onSyncWithRender() override;

private:

    Vec2 size;
    ColorB color;
    bool isSizeChanged;
    bool isColorChanged;
};

#endif /* __RENDER_SIMPLE_LOGIC_HPP__ */