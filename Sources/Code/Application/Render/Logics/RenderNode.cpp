#include "Render/Logics/RenderNode.hpp"

RenderNode::RenderNode() :
    isVisible(true),
    drawPriority(0) {
}

RenderNode::~RenderNode() {
}

bool RenderNode::serialize(const JSONNode& node) {
    return true;
}

bool RenderNode::init() {
    ET_SendEvent(&ETRender::ET_updateRenderQueue);
    ETNode<ETRenderNode>::connect(getEntityId());
    ETNode<ETRenderEvents>::connect(getEntityId());
    return true;
}

bool RenderNode::ET_isVisible() const {
    return isVisible;
}

void RenderNode::ET_hide() {
    isVisible = false;
}

void RenderNode::ET_show() {
    isVisible = true;
}

void RenderNode::ET_setDrawPriority(int newDrawPriority) {
    drawPriority = newDrawPriority;
    ET_SendEvent(&ETRender::ET_updateRenderQueue);
}

int RenderNode::ET_getDrawPriority() const {
    return drawPriority;
}