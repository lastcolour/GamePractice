#include "Logics/UIElementProxy.hpp"
#include "UIUtils.hpp"
#include "Render/ETRenderNode.hpp"

void UIElementProxy::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIElementProxy>("UIElementProxy")) {
        classInfo->addField("gridScale", &UIElementProxy::gridScale);
        classInfo->addField("useGridScale", &UIElementProxy::useGridScale);
    }
}

UIElementProxy::UIElementProxy() :
    gridScale(1.f),
    useGridScale(true) {
}

UIElementProxy::~UIElementProxy() {
}

void UIElementProxy::init() {
    UIElement::init();
    if(useGridScale) {
        ETNode<ETUIViewPortEvents>::connect(getEntityId());
    }
}

void UIElementProxy::onLoaded() {
    UIElement::onLoaded();
    applyNormScale();
}

void UIElementProxy::ET_onViewPortChanged(const Vec2i& newSize) {
    applyNormScale();
}

void UIElementProxy::onZIndexChanged(int newZIndex) {
    ET_SendEvent(getEntityId(), &ETRenderNode::ET_setZIndex, newZIndex);
}

void UIElementProxy::onHide(bool flag) {
    if(flag) {
        ET_SendEvent(getEntityId(), &ETRenderNode::ET_hide);
    } else {
        ET_SendEvent(getEntityId(), &ETRenderNode::ET_show);
    }
}

void UIElementProxy::onAlphaChanged(float newAlpha) {
    ET_SendEvent(getEntityId(), &ETRenderNode::ET_setAlphaMultiplier, newAlpha);
}

void UIElementProxy::applyNormScale() {
    if(useGridScale) {
        auto scaleFactor = UI::GetValueOnGrind(gridScale);
        ET_SendEvent(getEntityId(), &ETRenderNode::ET_setNormalizationScale, scaleFactor);
    }
}