#include "RenderGraph/RenderGraph.hpp"
#include "Nodes/Node.hpp"
#include "RenderContext.hpp"
#include "Platform/OpenGL.hpp"

#include <algorithm>
#include <cassert>

RenderGraph::RenderGraph() :
    needReorder(false) {
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

void RenderGraph::addChild(Node* node) {
    needReorder = true;
    node->setRenderGraph(this);
    children.push_back(node);
}

void RenderGraph::removeChild(Node* node) {
    auto it = std::find(children.begin(), children.end(), node);
    if(it != children.end()) {
        std::swap(*it, children.back());
        children.pop_back();
        needReorder = true;
    } else {
        assert(false && "Can't find child to remove");
    }
}

void RenderGraph::setNeedReorderNodes() {
    needReorder = true;
}

void RenderGraph::render() {
    if(needReorder) {
        std::sort(children.begin(), children.end(), [](Node* first, Node* second){
            return first->getDrawPriority() < second->getDrawPriority();
        });
        needReorder = false;
    }
    ctx.startFrame();
    for(auto node : children) {
        node->render();
    }
    ctx.endFrame();
}