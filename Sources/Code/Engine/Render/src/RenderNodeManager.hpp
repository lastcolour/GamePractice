#ifndef __RENDER_NODE_MANAGER_HPP__
#define __RENDER_NODE_MANAGER_HPP__

#include "Nodes/ETRenderNodeManager.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/SystemLogic.hpp"
#include "RenderGraph/RenderGraph.hpp"

#include <unordered_map>
#include <memory>

class Node;

class RenderNodeManager : public SystemLogic,
    public ETNode<ETRenderNodeManager> {
public:

    RenderNodeManager();
    virtual ~RenderNodeManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRenderNodeManager
    EntityId ET_createNode(const RenderNodeCreateParams& params) override;
    void ET_removeNode(EntityId nodeId) override;
    void ET_initRenderNode(Node* node) override;
    void ET_update() override;

private:

    RenderGraph renderGraph;
    std::unordered_map<EntityId, std::unique_ptr<Node>> nodes;
};

#endif /* __RENDER_NODE_MANAGER_HPP__ */