#include "RenderNodeManager.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Nodes/ImageNode.hpp"
#include "Nodes/NinePatchNode.hpp"
#include "Nodes/SimpleNode.hpp"
#include "Nodes/TextNode.hpp"
#include "Nodes/GradientNode.hpp"
#include "Nodes/ParticlesNode.hpp"
#include "Nodes/BlurNode.hpp"
#include "Render/ETRenderNode.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "RenderUtils.hpp"

#include <cassert>
#include <algorithm>

RenderNodeManager::RenderNodeManager() {
}

RenderNodeManager::~RenderNodeManager() {
}

bool RenderNodeManager::init() {
    ETNode<ETRenderNodeManager>::connect(getEntityId());
    ETNode<ETRenderContextEvents>::connect(getEntityId());

    if(RenderUtils::IsOpenGLContextExists()) {
        ET_onContextCreated();
    }

    return true;
}

void RenderNodeManager::deinit() {
}

void RenderNodeManager::ET_updateParticles(float dt) {
    for(auto node : particles) {
        node->update(dt);
    }
}

Node* RenderNodeManager::ET_createNode(const RenderNodeCreateParams& params) {
    std::unique_ptr<Node> node;
    switch(params.type) {
        case RenderNodeType::NinePatchImage: {
            node.reset(new NinePatchNode());
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
        case RenderNodeType::ParticleEmmiter: {
            node.reset(new ParticlesNode());
            break;
        }
        case RenderNodeType::Blur:
            node.reset(new BlurNode());
            break;
        default: {
            assert(false && "Invalid node type");
            return nullptr;
        }
    }
    node->preInit(params);
    auto resNode = node.get();
    ET_QueueEvent(&ETRenderNodeManager::ET_initRenderNode, node.release());
    return resNode;
}

void RenderNodeManager::ET_initRenderNode(Node* node) {
    nodes.insert({node, std::unique_ptr<Node>(node)});
    node->init();
    renderGraph.addChild(node);
    if(node->getType() == RenderNodeType::ParticleEmmiter) {
        particles.push_back(static_cast<ParticlesNode*>(node));
    }
}

void RenderNodeManager::ET_removeNode(Node* node) {
    auto it = nodes.find(node);
    if(it == nodes.end()) {
        return;
    }
    renderGraph.removeChild(it->second.get());
    if(node->getType() == RenderNodeType::ParticleEmmiter) {
        auto it = std::find(particles.begin(), particles.end(), node);
        particles.erase(it);
    }
    nodes.erase(it);
}

void RenderNodeManager::ET_onContextCreated() {
    renderGraph.init();
}

void RenderNodeManager::ET_onContextDestroyed() {
}

void RenderNodeManager::ET_drawFrame() {
    renderGraph.render();
}

void RenderNodeManager::ET_drawFrameToBuffer(ImageBuffer& imageBuffer, DrawContentFilter filter) {
    renderGraph.renderToBuffer(imageBuffer, filter);
}