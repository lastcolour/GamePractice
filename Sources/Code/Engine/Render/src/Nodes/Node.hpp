#ifndef __NODE_HPP__
#define __NODE_HPP__

#include "Nodes/ETRenderProxyNode.hpp"
#include "RenderContext.hpp"
#include "Render/RenderCommon.hpp"
#include "RenderMaterial.hpp"
#include "RenderGeometry.hpp"
#include "Core/ETPrimitives.hpp"
#include "Math/Transform.hpp"
#include "Nodes/ETRenderNodeManager.hpp"

class RenderGraph;

class Node : public ETNode<ETRenderProxyNode> {
public:

    Node();
    virtual ~Node();

    void initConnections(EntityId renderNodeId, const RenderNodeCreateParams& params);
    void initRender();
    void setRenderGraph(RenderGraph* graph);
    void render(RenderContext& ctx);
    int getDrawPriority() const;
    EntityId getNodeId() const;

    // ETRenderProxyNode
    void ET_setAlpha(float newAlpha) override;
    void ET_setDrawPriority(int newDrawPriority) override;
    void ET_setVisible(bool flag) override;
    void ET_setTransform(const Transform& newTm) override;
    void ET_setColor0(const ColorB& newColor) override {}
    void ET_setColor1(const ColorB& newColor) override {}
    void ET_setFontHeight(int newHeight) override {}
    void ET_setSize(const Vec2i& newSize) override {}
    void ET_setText(const std::string& newText) override {}
    void ET_setImage(const std::string& newImage) override {}
    void ET_setVertical(bool flag) override {}

protected:

    virtual void onInitConnections() {}
    virtual void onInitRender() = 0;
    virtual void onRender(RenderContext& ctx) = 0;
    virtual bool isVisible() const;

protected:

    void setBlendingMode(RenderBlendingType newBlending);
    void setMaterial(const char* matName);
    void setGeometry(PrimitiveGeometryType geomType);
    void setSetupAlpha(bool flag);

protected:

    EntityId nodeId;
    Transform tm;
    std::shared_ptr<RenderMaterial> mat;
    std::shared_ptr<RenderGeometry> geom;

private:

    RenderGraph* renderGraph;
    RenderBlendingType blending;
    float alpha;
    int drawPriority;
    bool visible;
    bool setupAlpha;
};

#endif /* __NODE_HPP__ */