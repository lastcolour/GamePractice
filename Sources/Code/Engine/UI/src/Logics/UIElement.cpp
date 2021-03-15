#include "Logics/UIElement.hpp"
#include "UI/ETUILayout.hpp"
#include "Reflect/ReflectContext.hpp"
#include "UIUtils.hpp"
#include "Core/ETLogger.hpp"
#include "UI/ETUIAnimation.hpp"

void UIElement::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIElement>("UIElement")) {
        classInfo->addField("hidden", &UIElement::isHidden);
        classInfo->addField("enabled", &UIElement::isEnabled);
    }
}

UIElement::UIElement() :
    alpha(1.f),
    parentAlpha(1.f),
    zIndex(0),
    isIgnoringTransform(false),
    isHidden(false),
    isEnabled(true),
    isParentHidden(false),
    isParentDisabled(false),
    isAddTmChanged(false) {
}

UIElement::~UIElement() {
}

void UIElement::init() {
    isIgnoringTransform = true;
    ETNode<ETUIElement>::connect(getEntityId());
    ETNode<ETEntityEvents>::connect(getEntityId());
    ETNode<ETUIAdditiveAnimationTarget>::connect(getEntityId());

    ET_SendEventReturn(layoutTm, getEntityId(), &ETEntity::ET_getLocalTransform);
}

void UIElement::deinit() {
}

void UIElement::ET_onLoaded() {
    isIgnoringTransform = false;
    zIndex -= 1;
    ET_setZIndex(zIndex + 1);
    if(isHidden) {
        isHidden = false;
        ET_hide();
    } else {
        isHidden = true;
        ET_show();
    }
}

void UIElement::ET_setHostLayout(EntityId newLayoutId) {
    hostLayoutId = newLayoutId;
    if (hostLayoutId == InvalidEntityId) {
        ET_setZIndex(0);
    }
}

EntityId UIElement::ET_getHostLayout() const {
    return hostLayoutId;
}

void UIElement::ET_setZIndex(int newZIndex) {
    if(zIndex == newZIndex) {
        return;
    }
    zIndex = newZIndex;
    onZIndexChanged(zIndex);
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onZIndexChanged, newZIndex);
}

int UIElement::ET_getZIndex() const {
    return zIndex;
}

int UIElement::ET_getZIndexDepth() const {
    return 1;
}

void UIElement::ET_setLayoutPos(const Vec2& layoutPt) {
    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getLocalTransform);
    layoutTm.pt = tm.pt;
}

void UIElement::updateHostLayout() {
    if(!ET_isHidden()) {
        ET_SendEvent(hostLayoutId, &ETUIElemAligner::ET_reAlign);
    }
}

void UIElement::updateSelfLayout() {
    if(!ET_isHidden()) {
        ET_SendEvent(getEntityId(), &ETUIElemAligner::ET_reAlign);
    }
}

void UIElement::ET_show() {
    if(!isHidden) {
        return;
    }
    isHidden = false;
    if(isParentHidden) {
        return;
    }
    onHide(false);
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onHidden, isHidden);
    if(!isParentHidden) {
        ET_SendEvent(hostLayoutId, &ETUIElemAligner::ET_reAlign);
    }
}

void UIElement::ET_hide() {
    if(isHidden) {
        return;
    }
    isHidden = true;
    if(isParentHidden) {
        return;
    }
    onHide(true);
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onHidden, isHidden);
    if(!isParentHidden) {
        ET_SendEvent(hostLayoutId, &ETUIElemAligner::ET_reAlign);
    }}

void UIElement::ET_setParentHidden(bool flag) {
    if(isParentHidden == flag) {
        return;
    }
    bool prevHidden = ET_isHidden();
    isParentHidden = flag;
    bool currHidden = ET_isHidden();
    if(prevHidden == currHidden) {
        return;
    }
    onHide(currHidden);
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onHidden, currHidden);
}

bool UIElement::ET_isHidden() const {
    return isHidden || isParentHidden;
}

float UIElement::ET_getAlpha() const {
    return alpha * parentAlpha;
}

void UIElement::ET_setAlpha(float newAlpha) {
    alpha = newAlpha;
    float resAlpha = alpha * parentAlpha * addTm.alpha;
    onAlphaChanged(resAlpha);
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onAlphaChanged, resAlpha);
}

void UIElement::ET_enable() {
    bool prevEnabled = ET_isEnabled();
    isEnabled = true;
    bool currEnabled = ET_isEnabled();
    if(prevEnabled == currEnabled) {
        return;
    }
    onDisabled(!currEnabled);
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onDisabled, !currEnabled);
}

void UIElement::ET_disable() {
    bool prevEnabled = ET_isEnabled();
    isEnabled = false;
    bool currEnabled = ET_isEnabled();
    if(prevEnabled == currEnabled) {
        return;
    }
    onDisabled(!currEnabled);
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onDisabled, !currEnabled);
}

void UIElement::ET_setParentDisabled(bool flag) {
    bool prevEnabled = ET_isEnabled();
    isParentDisabled = flag;
    bool currEnabled = ET_isEnabled();
    if(prevEnabled == currEnabled) {
        return;
    }
    onDisabled(!currEnabled);
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onDisabled, !currEnabled);
}

bool UIElement::ET_isEnabled() const {
    return isEnabled && !isParentDisabled;
}

void UIElement::ET_setIgnoreTransform(bool flag) {
    isIgnoringTransform = flag;
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onIngoreTransform, isIgnoringTransform);
}

void UIElement::ET_onTransformChanged(const Transform& newTm) {
    if(isIgnoringTransform) {
        return;
    }
    onTransformChanged(newTm);
}

void UIElement::ET_setParentAlpha(float newParentAlpha) {
    parentAlpha = newParentAlpha;
    ET_setAlpha(alpha);
}

void UIElement::ET_addAdditiveTransform(const AddtiveUITransform& newAddTm) {
    isAddTmChanged = true;
    addTm.combine(newAddTm);
}

void UIElement::ET_applyAdditiveTranform() {
    if(!isAddTmChanged) {
        return;
    }
    Transform resTm = addTm.applyTm(layoutTm);

    ET_setIgnoreTransform(true);
    ET_SendEvent(getEntityId(), &ETEntity::ET_setLocalTransform, resTm);
    ET_setIgnoreTransform(false);

    ET_setAlpha(alpha);
}

void UIElement::ET_resetAdditiveTransform() {
    addTm.reset();
    isAddTmChanged = false;
}