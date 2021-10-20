#include "Logics/RenderNode.hpp"
#include "Nodes/Node.hpp"

void RenderNode::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderNode>("RenderNode")) {
        classInfo->addField("isVisible", &RenderNode::isVisible);
        classInfo->addField("alpha", &RenderNode::alpha);
        classInfo->addField("zIndex", &RenderNode::zIndex);
    }
}

RenderNode::RenderNode(RenderNodeType nodeType) :
    proxyNode(nullptr),
    alpha(1.f),
    alphaMult(1.f),
    normScale(1.f),
    zIndex(0),
    type(nodeType),
    isVisible(true),
    isLoaded(false),
    emitEvents(false) {
}

RenderNode::~RenderNode() {
}

void RenderNode::init() {
    auto node = RenderUtils::CreateRenderNode(type);
    if(!node) {
        LogError("[RenderNode::init] Can't create proxy node for an enity: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        return;
    }

    proxyNode = node.get();

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);
    tm.scale *= normScale;
    alpha = Math::Clamp(alpha, 0.f, 1.f);

    proxyNode->setTransform(tm);
    proxyNode->setAlpha(alpha * alphaMult);
    proxyNode->setZIndex(zIndex);

    onInit();

    ET_QueueEvent(&ETRenderNodeManager::ET_initRenderNode, node.release());

    ETNode<ETRenderNode>::connect(getEntityId());
    ETNode<ETEntityEvents>::connect(getEntityId());
}

void RenderNode::deinit() {
    normScale = 1.f;
    ETNode<ETEntityEvents>::disconnect();
    ETNode<ETRenderNode>::disconnect();
    if(proxyNode != nullptr) {
        ET_QueueEvent(&ETRenderNodeManager::ET_removeNode, proxyNode);
        proxyNode = nullptr;
    }
}

void RenderNode::ET_setAlphaMultiplier(float newAlphaMult) {
    alphaMult = newAlphaMult;
    if(alphaMult < -0.f || alphaMult > 1.f) {
        LogWarning("[RenderNode::ET_setAlphaMultiplier] alpha-multiplier '%.1f' is out of range [0..1] (Entity: '%s')",
            alphaMult, EntityUtils::GetEntityName(getEntityId()));
        alphaMult = Math::Clamp(alphaMult, 0.f, 1.f);
    }
    float resAlpha = Math::Clamp(alphaMult * alpha, 0.f, 1.f);
    ET_QueueEvent(&ETRenderNodeManager::ET_scheduleNodeEvent, [node=proxyNode, resAlpha](){
        node->setAlpha(resAlpha);
    });
    if(emitEvents) {
        ET_SendEvent(getEntityId(), &ETRenderNodeEvents::ET_onAlphaMultChanged, newAlphaMult);
    }
}

float RenderNode::ET_getAlphaMultiplier() const {
    return alphaMult;
}

bool RenderNode::ET_isVisible() const {
    return isVisible;
}

void RenderNode::ET_hide() {
    if(!isVisible) {
        return;
    }
    isVisible = false;
    if(!isLoaded) {
        return;
    }
    ET_QueueEvent(&ETRenderNodeManager::ET_scheduleNodeEvent, [node=proxyNode](){
        node->setVisible(false);
    });
    if(emitEvents) {
        ET_SendEvent(getEntityId(), &ETRenderNodeEvents::ET_onHidden, !isVisible);
    }
}

void RenderNode::ET_show() {
    if(isVisible) {
        return;
    }
    isVisible = true;
    if(!isLoaded) {
        return;
    }
    ET_QueueEvent(&ETRenderNodeManager::ET_scheduleNodeEvent, [node=proxyNode](){
        node->setVisible(true);
    });
    if(emitEvents) {
        ET_SendEvent(getEntityId(), &ETRenderNodeEvents::ET_onHidden, !isVisible);
    }
}

void RenderNode::ET_setZIndex(int newZIndex) {
    if(newZIndex == zIndex) {
        return;
    }
    zIndex = newZIndex;
    ET_QueueEvent(&ETRenderNodeManager::ET_scheduleNodeEvent, [node=proxyNode, newZIndex](){
        node->setZIndex(newZIndex);
    });
    if(emitEvents) {
        ET_SendEvent(getEntityId(), &ETRenderNodeEvents::ET_onZIndexChanged, zIndex);
    }
}

int RenderNode::ET_getZIndex() const {
    return zIndex;
}

void RenderNode::ET_setStencilData(const StencilWirteReadData& newStencilData) {
    if(newStencilData.mode == stencilData.mode
        && newStencilData.refVal == stencilData.refVal) {
        return;
    }
    stencilData = newStencilData;
    ET_QueueEvent(&ETRenderNodeManager::ET_scheduleNodeEvent, [node=proxyNode, sData=newStencilData](){
        node->setStencilData(sData);
    });
}

void RenderNode::ET_onTransformChanged(const Transform& newTm) {
    ET_QueueEvent(&ETRenderNodeManager::ET_scheduleNodeEvent, [node=proxyNode, nScale=normScale, tm=newTm]()mutable{
        tm.scale *= nScale;
        node->setTransform(tm);
    });
}

void RenderNode::ET_setNormalizationScale(float newNormScale) {
    ET_QueueEvent(&ETRenderNodeManager::ET_scheduleNodeEvent, [node=proxyNode, newS=newNormScale, oldS=normScale](){
        float nScale = std::max(0.001f, newS / oldS);
        node->setNormScale(nScale);
    });
    normScale = newNormScale;
    if(emitEvents) {
        ET_SendEvent(getEntityId(), &ETRenderNodeEvents::ET_onNormScaleChanged, normScale);
    }
}

float RenderNode::ET_getNormalizationScale() const {
    return normScale;
}

void RenderNode::ET_onLoaded() {
    isLoaded = true;
    if(ET_isVisible()) {
        ET_QueueEvent(&ETRenderNodeManager::ET_scheduleNodeEvent, [node=proxyNode](){
            node->setVisible(true);
        });
    }
}

bool RenderNode::ET_setEmitEvents(bool flag) {
    emitEvents = flag;
    return true;
}