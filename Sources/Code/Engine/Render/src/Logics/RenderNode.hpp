#ifndef __RENDER_NODE_HPP__
#define __RENDER_NODE_HPP__

#include "Render/ETRenderNode.hpp"
#include "Entity/EntityLogic.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Core/ETPrimitives.hpp"
#include "RenderContext.hpp"
#include "Nodes/ETRenderNodeManager.hpp"
#include "Nodes/ETRenderProxyNode.hpp"
#include "Entity/ETEntity.hpp"

class RenderMaterial;
class RenderGeometry;
class RenderGraph;
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
    bool init() override;
    void deinit() override;

    // ETRenderNode
    bool ET_isVisible() const override;
    void ET_hide() override;
    void ET_show() override;
    void ET_setDrawPriority(int newDrawPriority) override;
    int ET_getDrawPriority() const override;
    void ET_setAlphaMultiplier(float newAlphaMult) override;
    void ET_setVisibilityMultiplier(bool newVisMult) override;

    // ETRenderProxyNodeEvents
    void ET_syncWithRender() override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override;
    void ET_onLoaded() override;

private:

    void markForSyncWithRender();

protected:

    EntityId renderNodeId;
    float alpha;
    float alphaMult;
    int drawPriority;
    RenderNodeType type;
    bool isVisible;
    bool visMult;
    bool isLoaded;
};

#endif /* __RENDER_NODE_HPP__ */