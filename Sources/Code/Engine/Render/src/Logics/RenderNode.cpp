#include "Logics/RenderNode.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Nodes/ETRenderNodeManager.hpp"
#include "Entity/ETEntity.hpp"
#include "Core/ETLogger.hpp"

#include <algorithm>

void RenderNode::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderNode>("RenderNode")) {
        classInfo->addField("isVisible", &RenderNode::isVisible);
        classInfo->addField("alpha", &RenderNode::alpha);
        classInfo->addField("drawPriority", &RenderNode::drawPriority);
    }
}

RenderNode::RenderNode(RenderNodeType nodeType) :
    proxyNode(nullptr),
    alpha(1.f),
    alphaMult(1.f),
    drawPriority(0),
    type(nodeType),
    isVisible(true),
    isLoaded(false),
    isTmChanged(false),
    isVisChanged(false) {
}

RenderNode::~RenderNode() {
}

bool RenderNode::init() {
    RenderNodeCreateParams params;

    ET_SendEventReturn(params.tm, getEntityId(), &ETEntity::ET_getTransform);
    params.alpha = std::min(1.f, std::max(0.f, alpha));
    params.drawPriority = drawPriority;
    params.type = type;
    ET_SendEventReturn(proxyNode, &ETRenderNodeManager::ET_createNode, params);
    if(!proxyNode) {
        LogError("[RenderNode::init] Can't create proxy node an enity: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        return false;
    }

    ETNode<ETRenderNode>::connect(getEntityId());
    ETNode<ETEntityEvents>::connect(getEntityId());

    return true;
}

void RenderNode::deinit() {
    ET_QueueEvent(&ETRenderNodeManager::ET_removeNode, proxyNode);
    proxyNode = nullptr;
}

void RenderNode::ET_setAlphaMultiplier(float newAlphaMult) {
    alphaMult = newAlphaMult;
    if(alphaMult < 0.f || alphaMult > 1.f) {
        LogWarning("[RenderNode::ET_setAlphaMultiplier] alpha-multiplier '%.1f' is out of range [0..1] (Entity: '%s'",
            alphaMult, EntityUtils::GetEntityName(getEntityId()));
        alphaMult = Math::Clamp(alphaMult, 0.f, 1.f);
    }
    isAlphaChanged = true;
    markForSyncWithRender();
}

bool RenderNode::ET_isVisible() const {
    return isVisible;
}

void RenderNode::ET_hide() {
    if(!isVisible) {
        return;
    }
    isVisible = false;
    isVisChanged = true;
    markForSyncWithRender();
}

void RenderNode::ET_show() {
    if(isVisible) {
        return;
    }
    isVisible = true;
    isVisChanged = true;
    markForSyncWithRender();
}

void RenderNode::ET_setDrawPriority(int newDrawPriority) {
    if(newDrawPriority != drawPriority) {
        drawPriority = newDrawPriority;
        isDrawPriorityChanged = true;
        markForSyncWithRender();
    }
}

int RenderNode::ET_getDrawPriority() const {
    return drawPriority;
}

void RenderNode::ET_onTransformChanged(const Transform& newTm) {
    isTmChanged = true;
    markForSyncWithRender();
}

void RenderNode::ET_onLoaded() {
    isLoaded = true;
    if(ET_isVisible()) {
        isVisChanged = true;
        markForSyncWithRender();
    }
}

void RenderNode::markForSyncWithRender() {
    if(!isLoaded) {
        return;
    }
    ETNode<ETRenderProxyNodeEvents>::connect(getEntityId());
}

void RenderNode::ET_syncWithRender() {
    onSyncWithRender();
    if(isTmChanged) {
        isTmChanged = false;
        Transform tm;
        ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
        proxyNode->setTransform(tm);
    }
    if(isVisChanged) {
        isVisChanged = false;
        proxyNode->setVisible(isVisible);
    }
    if(isAlphaChanged) {
        isAlphaChanged = false;
        auto newAlpha = std::min(1.f, std::max(0.f, alphaMult * alpha));
        proxyNode->setAlpha(newAlpha);
    }
    if(isDrawPriorityChanged) {
        isDrawPriorityChanged = false;
        proxyNode->setDrawPriority(drawPriority);
    }
    ETNode<ETRenderProxyNodeEvents>::disconnect();
}