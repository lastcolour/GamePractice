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

#include <atomic>

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
    void ET_setAlpha(float newAlpha) override;

    // ETRenderProxyNodeEvents
    void ET_syncTransform() override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override;
    void ET_onAllLogicsCreated() override {}

protected:

    EntityId renderNodeId;
    float alpha;
    int drawPriority;
    RenderNodeType type;
    std::atomic<bool> syncWithRender;
    bool isVisible;

};

#endif /* __RENDER_NODE_HPP__ */