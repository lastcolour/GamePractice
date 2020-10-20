#include "Logics/RenderNode.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Nodes/ETRenderProxyNode.hpp"
#include "Nodes/ETRenderNodeManager.hpp"
#include "Entity/ETEntity.hpp"

#include <algorithm>

void RenderNode::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderNode>("RenderNode")) {
        classInfo->addField("isVisible", &RenderNode::isVisible);
        classInfo->addField("alpha", &RenderNode::alpha);
        classInfo->addField("drawPriority", &RenderNode::drawPriority);
    }
}

RenderNode::RenderNode(RenderNodeType nodeType) :
    alpha(1.f),
    alphaMult(1.f),
    drawPriority(0),
    type(nodeType),
    isVisible(true),
    visMult(true),
    isLoaded(false) {
}

RenderNode::~RenderNode() {
}

bool RenderNode::init() {
    ETNode<ETRenderNode>::connect(getEntityId());
    ETNode<ETEntityEvents>::connect(getEntityId());

    RenderNodeCreateParams params;
    ET_SendEventReturn(params.tm, getEntityId(), &ETEntity::ET_getTransform);
    params.alpha = std::min(1.f, std::max(0.f, alpha));
    params.drawPriority = drawPriority;
    params.type = type;
    ET_SendEventReturn(renderNodeId, &ETRenderNodeManager::ET_createNode, params);

    return true;
}

void RenderNode::deinit() {
    ET_QueueEvent(&ETRenderNodeManager::ET_removeNode, renderNodeId);
    renderNodeId = InvalidEntityId;
}

void RenderNode::ET_setAlphaMultiplier(float newAlphaMult) {
    alphaMult = newAlphaMult;
    auto newAlpha = alpha * alphaMult;
    newAlpha = std::min(1.f, std::max(0.f, newAlpha));
    ET_QueueEvent(renderNodeId, &ETRenderProxyNode::ET_setAlpha, newAlpha);
}

bool RenderNode::ET_isVisible() const {
    return isVisible && visMult;
}

void RenderNode::ET_hide() {
    if(!isVisible) {
        return;
    }
    isVisible = false;
    ET_QueueEvent(renderNodeId, &ETRenderProxyNode::ET_setVisible, false);
}

void RenderNode::ET_show() {
    if(isVisible) {
        return;
    }
    isVisible = true;
    markForSyncWithRender();
}

void RenderNode::ET_setVisibilityMultiplier(bool newVisMult) {
    if(visMult == newVisMult) {
        return;
    }
    visMult = newVisMult;
    markForSyncWithRender();
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
    markForSyncWithRender();
}

void RenderNode::ET_onLoaded() {
    isLoaded = true;
    markForSyncWithRender();
}

void RenderNode::markForSyncWithRender() {
    if(!isLoaded) {
        return;
    }
    ETNode<ETRenderProxyNodeEvents>::connect(getEntityId());
}

void RenderNode::ET_syncWithRender() {
    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    ET_SendEvent(renderNodeId, &ETRenderProxyNode::ET_setTransform, tm);
    auto visFlag = ET_isVisible();
    ET_SendEvent(renderNodeId, &ETRenderProxyNode::ET_setVisible, visFlag);
    ETNode<ETRenderProxyNodeEvents>::disconnect();
}