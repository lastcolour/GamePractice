#ifndef __NODE_HPP__
#define __NODE_HPP__

#include "RenderGraph/RenderContext.hpp"
#include "Render/RenderCommon.hpp"
#include "RenderShader.hpp"
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
    Node* getMaskNode();

    void setAlpha(float newAlpha);
    void setDrawPriority(int newDrawPriority);
    void setVisible(bool flag);
    void setTransform(const Transform& newTm);
    void setMaskNode(Node* newMaskNode);

protected:

    virtual void onInit() = 0;
    virtual void onRender(RenderContext& ctx) = 0;
    virtual bool isVisible() const;

protected:

    void setBlendingMode(RenderBlendingType newBlending);
    void setShader(const char* shaderName);
    void setGeometry(PrimitiveGeometryType geomType);

protected:

    Transform tm;
    std::shared_ptr<RenderShader> shader;
    std::shared_ptr<RenderGeometry> geom;

private:

    RenderGraph* renderGraph;
    Node* maskNode;
    RenderBlendingType blending;
    RenderNodeType type;
    float alpha;
    int drawPriority;
    bool visible;
};

#endif /* __NODE_HPP__ */