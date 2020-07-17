#ifndef __RENDER_NODE_HPP__
#define __RENDER_NODE_HPP__

#include "Render/ETRenderNode.hpp"
#include "Entity/EntityLogic.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Core/ETPrimitives.hpp"
#include "RenderContext.hpp"

class RenderMaterial;
class RenderGeometry;
class RenderGraph;

class RenderNode : public EntityLogic,
    public ETNode<ETRenderNode> {
public:

    RenderNode();
    virtual ~RenderNode();

    void setRenderGraph(RenderGraph* graph);
    void render();

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
    void ET_setAlpha(float newAlpha) override;

protected:

    virtual void onRender(RenderContext& ctx) = 0;

protected:

    void setBlendingMode(RenderBlendingType newBlending);

protected:

    std::shared_ptr<RenderMaterial> mat;
    std::shared_ptr<RenderGeometry> geom;

private:

    RenderGraph* renderGraph;
    float alpha;
    int drawPriority;
    RenderBlendingType blending;
    bool isVisible;
};

#endif /* __RENDER_NODE_HPP__ */