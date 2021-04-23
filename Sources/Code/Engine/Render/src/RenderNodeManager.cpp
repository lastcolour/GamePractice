#include "RenderNodeManager.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Nodes/Node.hpp"
#include "RenderUtils.hpp"

#include <cassert>

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
    ET_PollAllEvents<ETRenderNodeManager>();
}

void RenderNodeManager::ET_addUpdateEvent(std::function<void(void)> func) {
    func();
}

void RenderNodeManager::ET_initRenderNode(Node* node) {
    nodes.insert({node, std::unique_ptr<Node>(node)});
    node->init();
    renderGraph.addChild(node);
}

void RenderNodeManager::ET_removeNode(Node* node) {
    auto it = nodes.find(node);
    if(it == nodes.end()) {
        return;
    }
    renderGraph.removeChild(it->second.get());
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

void RenderNodeManager::ET_drawFrameToBuffer(void* outBuffer, DrawContentFilter filter) {
    renderGraph.renderToBuffer(outBuffer, filter);
}