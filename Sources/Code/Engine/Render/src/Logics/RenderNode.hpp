#ifndef __RENDER_NODE_HPP__
#define __RENDER_NODE_HPP__

#include "Entity/EntityLogic.hpp"
#include "Render/RenderCommon.hpp"
#include "Render/ETRenderNode.hpp"
#include "RenderUtils.hpp"
#include "Nodes/ETRenderNodeManager.hpp"

class RenderNode : public EntityLogic,
    public ETNode<ETRenderNode>,
    public ETNode<ETEntityEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderNode(RenderNodeType nodeType);
    virtual ~RenderNode() = 0;

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETRenderNode
    bool ET_isVisible() const override;
    void ET_hide() override;
    void ET_show() override;
    void ET_setDrawPriority(int newDrawPriority) override;
    int ET_getDrawPriority() const override;
    void ET_setAlphaMultiplier(float newAlphaMult) override;
    void ET_setStencilData(const StencilWirteReadData& newSteniclData) override;
    void ET_setNormalizationScale(float newNormScale) override;
    float ET_getNormalizationScale() const override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override;
    void ET_onLoaded() override;

protected:

    virtual void onInit() = 0;

protected:

    StencilWirteReadData stencilData;
    Node* proxyNode;
    float alpha;
    float alphaMult;
    float normScale;
    int drawPriority;
    RenderNodeType type;
    bool isVisible;
    bool isLoaded;
};

#endif /* __RENDER_NODE_HPP__ */