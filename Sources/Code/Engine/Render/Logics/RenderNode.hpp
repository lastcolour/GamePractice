#ifndef __RENDER_NODE_HPP__
#define __RENDER_NODE_HPP__

#include "Render/ETRenderInterfaces.hpp"
#include "Entity/EntityLogic.hpp"

class RenderNode : public EntityLogic,
    public ETNode<ETRenderNode>,
    public ETNode<ETRenderEvents> {
public:

    RenderNode();
    virtual ~RenderNode();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETRenderNode
    bool ET_isVisible() const override;
    void ET_hide() override;
    void ET_show() override;
    void ET_setDrawPriority(int newDrawPriority) override;
    int ET_getDrawPriority() const override;
    void ET_setMaterial(const char* matName) override;
    void ET_setGeometry(PrimitiveGeometryType geomType) override;

    // ETRenderEvents
    void ET_onRenderPortResized() override {}
    void ET_onRender(RenderContext& renderCtx) override;

protected:

    virtual void onRender(RenderContext& ctx) = 0;

protected:

    std::shared_ptr<RenderMaterial> mat;
    std::shared_ptr<RenderGeometry> geom;

private:

    int drawPriority;
    bool isVisible;
};

#endif /* __RENDER_NODE_HPP__ */