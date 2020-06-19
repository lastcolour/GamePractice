#ifndef __RENDER_SIMPLE_LOGIC_HPP__
#define __RENDER_SIMPLE_LOGIC_HPP__

#include "Logics/RenderNode.hpp"

#include <memory>

class RenderMaterial;
class RenderGeometry;

class RenderSimpleLogic : public RenderNode,
    public ETNode<ETRenderSimpleLogic>,
    public ETNode<ETRenderRect> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderSimpleLogic();
    virtual ~RenderSimpleLogic();

    // EntityLogic
    bool init() override;

    // ETRenderSimpleLogic
    void ET_setColor(const ColorB& col) override;

    // ETRenderRect
    void ET_setSize(const Vec2i& newSize) override;
    Vec2i ET_getSize() const override;

protected:

    // RenderNode
    void onRender(RenderContext& renderCtx) override;

private:

    Vec2i size;
    ColorB color;
};

#endif /* __RENDER_SIMPLE_LOGIC_HPP__ */