#ifndef __RENDER_NODE_MANAGER_HPP__
#define __RENDER_NODE_MANAGER_HPP__

#include "Nodes/ETRenderNodeManager.hpp"
#include "Core/SystemLogic.hpp"
#include "RenderGraph/RenderGraph.hpp"

class ParticlesNode;

class RenderNodeManager : public SystemLogic,
    public ETNode<ETRenderNodeManager>,
    public ETNode<ETRenderContextEvents> {
public:

    RenderNodeManager();
    virtual ~RenderNodeManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRenderNodeManager
    void ET_addUpdateEvent(std::function<void(void)> func) override;
    void ET_removeNode(Node* node) override;
    void ET_initRenderNode(Node* node) override;
    void ET_drawFrame() override;
    void ET_drawFrameToBuffer(void* outBuffer, DrawContentFilter filter) override;

    // ETRenderContextEvents
    void ET_onContextCreated() override;
    void ET_onContextDestroyed() override;

private:

    RenderGraph renderGraph;
    std::unordered_map<Node*, std::unique_ptr<Node>> nodes;
};

#endif /* __RENDER_NODE_MANAGER_HPP__ */