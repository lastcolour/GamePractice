#ifndef __NODE_HPP__
#define __NODE_HPP__

#include "RenderContext.hpp"
#include "Render/RenderCommon.hpp"
#include "RenderMaterial.hpp"
#include "RenderGeometry.hpp"
#include "Core/ETPrimitives.hpp"
#include "Math/Transform.hpp"
#include "Nodes/ETRenderNodeManager.hpp"

class RenderGraph;

class Node {
public:

    Node();
    virtual ~Node();

    void preInit(const RenderNodeCreateParams& params);
    void init();
    void setRenderGraph(RenderGraph* graph);
    void render(RenderContext& ctx);
    int getDrawPriority() const;
    RenderNodeType getType() const;

    void setAlpha(float newAlpha);
    void setDrawPriority(int newDrawPriority);
    void setVisible(bool flag);
    void setTransform(const Transform& newTm);

protected:

    virtual void onInit() = 0;
    virtual void onRender(RenderContext& ctx) = 0;
    virtual bool isVisible() const;

protected:

    void setBlendingMode(RenderBlendingType newBlending);
    void setMaterial(const char* matName);
    void setGeometry(PrimitiveGeometryType geomType);

protected:

    Transform tm;
    std::shared_ptr<RenderMaterial> mat;
    std::shared_ptr<RenderGeometry> geom;

private:

    RenderGraph* renderGraph;
    RenderBlendingType blending;
    RenderNodeType type;
    float alpha;
    int drawPriority;
    bool visible;
};

#endif /* __NODE_HPP__ */