#ifndef __NODE_HPP__
#define __NODE_HPP__

#include "RenderGraph/RenderContext.hpp"
#include "RenderShader.hpp"
#include "RenderGeometry.hpp"
#include "RenderUtils.hpp"

class RenderGraph;

class Node {
public:

    Node();
    virtual ~Node();

    void init();
    void setRenderGraph(RenderGraph* graph);
    void render(RenderContext& ctx);
    int getDrawPriority() const;

    void setAlpha(float newAlpha);
    void setDrawPriority(int newDrawPriority);
    void setVisible(bool flag);
    void setTransform(const Transform& newTm);
    void setStencilData(const StencilWirteReadData& newStencilData);
    void setBlendingMode(const BlendMode& newBlendMode);
    void setNormScale(float scale);

    virtual bool canRender() const;

protected:

    virtual void onRenderStart(RenderContext& ctx);
    virtual void onRenderEnd(RenderContext& ctx);

    virtual Mat4 calcModelMat(const Transform& tm) = 0;
    virtual void onRender(RenderContext& ctx) = 0;
    virtual void onInit() = 0;

protected:

    void setShader(const char* shaderName);
    void setGeometry(PrimitiveGeometryType geomType);
    void setModelMatDirty();

protected:

    Transform tm;
    Mat4 modelMat;
    std::shared_ptr<RenderShader> shader;
    std::shared_ptr<RenderGeometry> geom;

private:

    RenderGraph* renderGraph;
    StencilWirteReadData stencilData;
    BlendMode blendMode;
    float alpha;
    int drawPriority;
    bool visible;
    bool modelMatDirty;
};

#endif /* __NODE_HPP__ */