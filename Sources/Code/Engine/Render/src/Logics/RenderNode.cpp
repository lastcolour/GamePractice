#include "Logics/RenderNode.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Nodes/ETRenderProxyNode.hpp"
#include "Nodes/ETRenderNodeManager.hpp"
#include "Entity/ETEntity.hpp"

void RenderNode::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderNode>("RenderNode")) {
        classInfo->addField("isVisible", &RenderNode::isVisible);
        classInfo->addField("alpha", &RenderNode::alpha);
        classInfo->addField("drawPriority", &RenderNode::drawPriority);
    }
}

RenderNode::RenderNode(RenderNodeType nodeType) :
    alpha(1.f),
    drawPriority(0),
    type(nodeType),
    isVisible(true),
    isTransformUpdated(false) {
}

RenderNode::~RenderNode() {
}

bool RenderNode::init() {
    ETNode<ETRenderNode>::connect(getEntityId());
    ETNode<ETEntityEvents>::connect(getEntityId());
    ETNode<ETRenderProxyNodeEvents>::connect(getEntityId());

    RenderNodeCreateParams params;
    ET_SendEventReturn(params.tm, getEntityId(), &ETEntity::ET_getTransform);
    params.alpha = alpha;
    params.drawPriority = drawPriority;
    params.type = type;
    ET_SendEventReturn(renderNodeId, &ETRenderNodeManager::ET_createNode, params);

    return true;
}

void RenderNode::deinit() {
    ET_QueueEvent(&ETRenderNodeManager::ET_removeNode, renderNodeId);
    renderNodeId = InvalidEntityId;
}

void RenderNode::ET_setAlpha(float newAlpha) {
    alpha = newAlpha;
    ET_QueueEvent(renderNodeId, &ETRenderProxyNode::ET_setAlpha, newAlpha);
}

bool RenderNode::ET_isVisible() const {
    return isVisible;
}

void RenderNode::ET_hide() {
    isVisible = false;
    ET_QueueEvent(renderNodeId, &ETRenderProxyNode::ET_setVisible, isVisible);
}

void RenderNode::ET_show() {
    isVisible = true;
    ET_QueueEvent(renderNodeId, &ETRenderProxyNode::ET_setVisible, isVisible);
}

void RenderNode::ET_setDrawPriority(int newDrawPriority) {
    if(newDrawPriority != drawPriority) {
        drawPriority = newDrawPriority;
        ET_QueueEvent(renderNodeId, &ETRenderProxyNode::ET_setDrawPriority, newDrawPriority);
    }
}

int RenderNode::ET_getDrawPriority() const {
    return drawPriority;
}

void RenderNode::ET_onTransformChanged(const Transform& newTm) {
    isTransformUpdated.store(true);
}

void RenderNode::ET_syncTransform() {
    bool isChanged = true;
    if(isTransformUpdated.compare_exchange_strong(isChanged, false)) {
        Transform tm;
        ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
        ET_QueueEvent(renderNodeId, &ETRenderProxyNode::ET_setTransform, tm);
    }
}