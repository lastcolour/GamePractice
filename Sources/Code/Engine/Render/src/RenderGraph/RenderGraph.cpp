#include "RenderGraph/RenderGraph.hpp"
#include "Logics/RenderNode.hpp"
#include "RenderContext.hpp"
#include "Platform/OpenGL.hpp"

#include <algorithm>

RenderGraph::RenderGraph() {
}

RenderGraph::~RenderGraph() {
    for(auto node : children) {
        node->setRenderGraph(nullptr);
    }
}

RenderContext& RenderGraph::getContext() {
    return ctx;
}

const RenderContext& RenderGraph::getContext() const {
    return ctx;
}

void RenderGraph::addChild(RenderNode* node) {
    needReorder = true;
    node->setRenderGraph(this);
    children.push_back(node);
}

void RenderGraph::removeChild(RenderNode* node) {
    auto it = std::find(children.begin(), children.end(), node);
    if(it != children.end()) {
        std::swap(*it, children.back());
        children.pop_back();
        needReorder = true;
    }
}

void RenderGraph::reorderNodes() {
    needReorder = true;
}

void RenderGraph::render() {
    if(needReorder) {
        std::sort(children.begin(), children.end(), [](RenderNode* first, RenderNode* second){
            return first->ET_getDrawPriority() < second->ET_getDrawPriority();
        });
        needReorder = false;
    }
    ctx.startFrame();
    for(auto node : children) {
        node->render();
    }
    ctx.endFrame();
}