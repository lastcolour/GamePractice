#include "RenderNodeManager.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Nodes/ImageNode.hpp"
#include "Nodes/ColoredTextureNode.hpp"
#include "Nodes/SimpleNode.hpp"
#include "Nodes/TextNode.hpp"
#include "Nodes/GradientNode.hpp"
#include "Render/ETRenderNode.hpp"
#include "Render/ETRenderInterfaces.hpp"

#include <cassert>

RenderNodeManager::RenderNodeManager() {
}

RenderNodeManager::~RenderNodeManager() {
}

bool RenderNodeManager::init() {
    ETNode<ETRenderNodeManager>::connect(getEntityId());
    return true;
}

void RenderNodeManager::deinit() {
}

EntityId RenderNodeManager::ET_createNode(const RenderNodeCreateParams& params) {
    auto nodeId = GetETSystem()->createNewEntityId();
    std::unique_ptr<Node> node;
    switch(params.type) {
        case RenderNodeType::ColoredTexture: {
            node.reset(new ColoredTextureNode());
            break;
        }
        case RenderNodeType::Gradient: {
            node.reset(new GradientNode());
            break;
        }
        case RenderNodeType::Image: {
            node.reset(new ImageNode());
            break;
        }
        case RenderNodeType::Simple: {
            node.reset(new SimpleNode());
            break;
        }
        case RenderNodeType::Text: {
            node.reset(new TextNode());
            break;
        }
        default: {
            assert(false && "Invalid node type");
            return InvalidEntityId;
        }
    }
    node->initConnection(nodeId, params);
    if(Render::IsRenderThread()) {
        ET_initRenderNode(node.release());
    } else {
        ET_QueueEvent(&ETRenderNodeManager::ET_initRenderNode, node.release());
    }
    return nodeId;
}

void RenderNodeManager::ET_initRenderNode(Node* node) {
    nodes.insert({node->getNodeId(), std::unique_ptr<Node>(node)});
    node->initRender();
    renderGraph.addChild(node);
}

void RenderNodeManager::ET_removeNode(EntityId nodeId) {
    if(!Render::IsRenderThread()) {
        ET_QueueEvent(&ETRenderNodeManager::ET_removeNode, nodeId);
        return;
    }
    auto it = nodes.find(nodeId);
    if(it == nodes.end()) {
        return;
    }
    renderGraph.removeChild(it->second.get());
    nodes.erase(it);
}

void RenderNodeManager::ET_update() {
    ET_PollAllEvents<ETRenderNodeManager>();
    ET_PollAllEvents<ETRenderProxyNode>();
    ET_SendEvent(&ETRenderProxyNodeEvents::ET_syncTransform);

    renderGraph.render();
}