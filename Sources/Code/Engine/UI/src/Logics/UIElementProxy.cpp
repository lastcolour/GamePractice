#include "Logics/UIElementProxy.hpp"
#include "UIUtils.hpp"
#include "Render/ETRenderNode.hpp"

void UIProxyNode::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIProxyNode>("UIProxyNode")) {
        classInfo->addField("target", &UIProxyNode::entId);
        classInfo->addField("normScale", &UIProxyNode::normScale);
        classInfo->addField("extraZOffset", &UIProxyNode::extraZOffset);
    }
}

UIProxyNode::UIProxyNode() :
    normScale(1.f),
    extraZOffset(1) {
}

void UIElementProxy::Reflect(ReflectContext& ctx) {
    UIElementProxy obj;
    if(auto classInfo = ctx.classInfo<UIElementProxy>("UIElementProxy")) {
        classInfo->addField("proxies", &UIElementProxy::nodes);
    }
}

UIElementProxy::UIElementProxy() {
}

UIElementProxy::~UIElementProxy() {
}

void UIElementProxy::init() {
    UIElement::init();
    ETNode<ETUIViewPortEvents>::connect(getEntityId());
}

void UIElementProxy::deinit() {
    UIElement::deinit();
}

void UIElementProxy::ET_onLoaded() {
    UIElement::ET_onLoaded();
    applyNormScale();
}

void UIElementProxy::ET_onViewPortChanged(const Vec2i& newSize) {
    applyNormScale();
}

AABB2D UIElementProxy::ET_getBox() const {
    return AABB2D(0.f);
}

UIBoxMargin UIElementProxy::ET_getMargin() const {
    return UIBoxMargin();
}

void UIElementProxy::onZIndexChanged(int newZIndex) {
    for(auto& node : nodes) {
        int nodeZIndex = newZIndex + node.extraZOffset;
        ET_SendEvent(node.entId, &ETRenderNode::ET_setDrawPriority, nodeZIndex);
    }
}

void UIElementProxy::onHide(bool flag) {
    if(flag) {
        for(auto& node : nodes) {
            ET_SendEvent(node.entId, &ETRenderNode::ET_hide);
        }
    } else {
        for(auto& node : nodes) {
            ET_SendEvent(node.entId, &ETRenderNode::ET_show);
        }
    }
}

void UIElementProxy::onAlphaChanged(float newAlpha) {
    for(auto& node : nodes) {
        ET_SendEvent(node.entId, &ETRenderNode::ET_setAlphaMultiplier, newAlpha);
    }
}

void UIElementProxy::applyNormScale() {
    for(auto& node : nodes) {
        float normScale = UI::GetValueOnGrind(node.normScale);
        ET_SendEvent(node.entId, &ETRenderNode::ET_setNormalizationScale, normScale);
    }
}