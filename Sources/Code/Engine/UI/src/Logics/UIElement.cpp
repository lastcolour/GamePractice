#include "Logics/UIElement.hpp"
#include "UI/ETUILayout.hpp"
#include "UIUtils.hpp"
#include "UI/ETUIAnimation.hpp"

#include <cassert>

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
    isHidden(false),
    isEnabled(true),
    isParentHidden(false),
    isParentDisabled(false),
    isAddTmChanged(false) {
}

UIElement::~UIElement() {
}

void UIElement::init() {
    if(ET_IsExistNode<ETUIElement>(getEntityId())) {
        LogError("[UIElement::init] There is another UIElement on entity: '%s'",
            getEntityName());
    }

    ETNode<ETUIElement>::connect(getEntityId());
    ETNode<ETUIAdditiveAnimationTarget>::connect(getEntityId());

    layoutTm = getLocalTransform();
}

void UIElement::deinit() {
    ETNode<ETUIElement>::disconnect();
}

void UIElement::onLoaded() {
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
    assert(hostLayoutId != getEntityId());
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

void UIElement::ET_setLayoutPos(const Vec2& layoutPt) {
    Transform tm = getLocalTransform();
    layoutTm.pt = tm.pt;
}

void UIElement::updateHostLayout() {
    if(!ET_isHidden() && hostLayoutId.isValid()) {
        ET_SendEvent(&ETUIReAlignManager::ET_setLayoutDirty, hostLayoutId);
    }
}

void UIElement::updateSelfLayout() {
    if(!ET_isHidden()) {
        ET_SendEvent(&ETUIReAlignManager::ET_setLayoutDirty, getEntityId());
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
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onHidden, false);
    if(!isParentHidden) {
        updateHostLayout();
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
    ET_SendEvent(getEntityId(), &ETUIElementEvents::ET_onHidden, true);
    if(!isParentHidden) {
        updateHostLayout();
    }
}

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
    Transform resTm = addTm.tm;
    resTm.addDelta(layoutTm);

    setLocalTransform(resTm);

    ET_setAlpha(alpha);
}

void UIElement::ET_resetAdditiveTransform() {
    addTm.reset();
    isAddTmChanged = false;
}