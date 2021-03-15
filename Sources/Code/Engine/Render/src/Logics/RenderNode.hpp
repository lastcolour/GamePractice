#ifndef __RENDER_NODE_HPP__
#define __RENDER_NODE_HPP__

#include "Render/ETRenderNode.hpp"
#include "Entity/EntityLogic.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Core/ETPrimitives.hpp"
#include "Nodes/ETRenderNodeManager.hpp"
#include "Entity/ETEntity.hpp"
#include "Nodes/Node.hpp"

class ReflectContext;

class RenderNode : public EntityLogic,
    public ETNode<ETRenderNode>,
    public ETNode<ETEntityEvents>,
    public ETNode<ETRenderProxyNodeEvents> {
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

    // ETRenderProxyNodeEvents
    void ET_syncWithRender() override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override;
    void ET_onLoaded() override;

protected:

    virtual void onSyncWithRender() = 0;

protected:

    void markForSyncWithRender();

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
    bool isTmChanged;
    bool isVisChanged;
    bool isDrawPriorityChanged;
    bool isAlphaChanged;
    bool isStencilDataChanged;
    bool isMarkedForSync;
};

#endif /* __RENDER_NODE_HPP__ */