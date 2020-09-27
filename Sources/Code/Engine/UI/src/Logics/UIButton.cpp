#include "Logics/UIButton.hpp"
#include "UI/ETUIBox.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Reflect/EnumInfo.hpp"
#include "UI/ETUIAnimation.hpp"
#include "Render/ETRenderNode.hpp"
#include "UIUtils.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>

namespace {

const float MOVE_AWAY_GRID_LEN = 1.f;
const float TIME_OUT_SEC_DUR = 1000.f;

bool isMoveAway(const Vec2i& pressPt, const Vec2i currPt) {
    auto diff = currPt - pressPt;

    Vec2 shift(0);
    shift.x = UI::CovertValueFromGrid(diff.y);
    shift.y = UI::CovertValueFromGrid(diff.x);

    if(shift.getLenght() > MOVE_AWAY_GRID_LEN) {
        return true;
    }

    return false;
}

bool isPressTimeRunOut(const TimePoint& pressTime) {
    auto now = TimePoint::GetNowTime();
    auto dt = now.getSecondsElapsedFrom(pressTime);
    if(dt > TIME_OUT_SEC_DUR) {
        return true;
    }
    return false;
}

} // namespace

void UIButton::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<UIEventType>("UIEventType")) {
        enumInfo->addValues<UIEventType>({
            {"None", UIEventType::None},
            {"OnMainViewStartGame", UIEventType::OnMainViewStartGame},
            {"OnGameEndViewExit", UIEventType::OnGameEndViewExit},
            {"OnBackButton", UIEventType::OnBackButton},
            {"OnGameGameEnd", UIEventType::OnGameGameEnd},
            {"OnSurfaceHidden", UIEventType::OnSurfaceHidden},
            {"OnSurfaceShown", UIEventType::OnSurfaceShown},
            {"OnPauseViewResume", UIEventType::OnPauseViewResume},
            {"OnPauseViewRestart", UIEventType::OnPauseViewRestart},
            {"OnPauseViewExit", UIEventType::OnPauseViewExit}
        });
    }
    if(auto classInfo = ctx.classInfo<UIButton>("UIButton")) {
        classInfo->addBaseClass<UIBox>();
        classInfo->addField("label", &UIButton::labelId);
        classInfo->addField("event", &UIButton::eventType);
    }
}

UIButton::UIButton() :
    pressPt(0),
    eventType(UIEventType::None) {
}

UIButton::~UIButton() {
}

bool UIButton::init() {
    if(labelId == getEntityId()) {
        LogWarning("[UIButton::init] Label should be other entity than this: '%s'", EntityUtils::GetEntityName(getEntityId()));
        labelId = InvalidEntityId;
    }
    UIBox::init();
    ETNode<ETUIInteractionBox>::connect(getEntityId());
    ETNode<ETUIAnimationEvents>::connect(getEntityId());
    return true;
}

void UIButton::deinit() {
    UIBox::deinit();
    ETNode<ETUIInteractionBox>::disconnect();
    ETNode<ETUIAnimationEvents>::disconnect();
}

EInputEventResult UIButton::ET_onInputEvent(EActionType type, const Vec2i& pt) {
    EInputEventResult res;
    switch(type) {
        case EActionType::Press:
            res = onPress(pt);
            break;
        case EActionType::Move:
            res = onMove(pt);
            break;
        case EActionType::Release:
            res = onRelease(pt);
            break;
    }
    return res;
}

bool UIButton::canContinueEvent(const Vec2i& pt) const {
    if(isPressTimeRunOut(pressTime)) {
        LogDebug("[UIButton::canContinueEvent] Press time run away");
        return false;
    }
    if(isMoveAway(pressPt, pt)) {
        LogDebug("[UIButton::canContinueEvent] Cursor moved-away");
        return false;
    }
    if(!UI::IsInsideBox(pt, ET_getHitBox())) {
        LogDebug("[UIButton::canContinueEvent] Cursor moved out of the hit box");
        return false;
    }
    return true;
}

EInputEventResult UIButton::onPress(const Vec2i& pt) {
    EntityId activeBtId;
    ET_SendEventReturn(activeBtId, &ETUIButtonEventManager::ET_getActiveButton);
    if(activeBtId.isValid()) {
        return EInputEventResult::Ignore;
    }
    pressPt = pt;
    pressTime = TimePoint::GetNowTime();
    return EInputEventResult::Accept;
}

EInputEventResult UIButton::onMove(const Vec2i& pt) {
    if(!canContinueEvent(pt)) {
        return EInputEventResult::Ignore;
    }
    return EInputEventResult::Accept;
}

EInputEventResult UIButton::onRelease(const Vec2i& pt) {
    if(!canContinueEvent(pt)) {
        return EInputEventResult::Ignore;
    }
    if(!ET_IsExistNode<ETUIAnimation>(getEntityId())) {
        ET_SendEvent(&ETUIEventManager::ET_onEvent, eventType);
    } else {
        ET_SendEvent(&ETUIButtonEventManager::ET_setActiveButton, getEntityId());
        ET_SendEvent(getEntityId(), &ETUIAnimation::ET_start);
    }
    return EInputEventResult::Accept;
}

AABB2Di UIButton::ET_getHitBox() const {
    return ET_getBox();
}

void UIButton::ET_onAnimationEnd() {
    {
        EntityId activeBtId;
        ET_SendEventReturn(activeBtId, &ETUIButtonEventManager::ET_getActiveButton);
        assert(activeBtId == getEntityId());
    }
    ET_SendEvent(&ETUIButtonEventManager::ET_setActiveButton, InvalidEntityId);
    ET_SendEvent(&ETUIEventManager::ET_onEvent, eventType);
}

void UIButton::onZIndexChanged(int newZIndex) {
    UIBox::onZIndexChanged(newZIndex);
    auto labelZIndex = newZIndex + 1;
    ET_SendEvent(labelId, &ETUIElement::ET_setZIndex, labelZIndex);
    ET_SendEvent(labelId, &ETRenderNode::ET_setDrawPriority, labelZIndex);
}

int UIButton::ET_getZIndexDepth() const {
    return 2;
}

void UIButton::onHide(bool flag) {
    UIBox::onHide(flag);
    if(flag) {
        ET_SendEvent(labelId, &ETRenderNode::ET_hide);
    } else {
        ET_SendEvent(labelId, &ETRenderNode::ET_show);
    }
}

void UIButton::onAlphaChanged(float newAlpha) {
    UIBox::onAlphaChanged(newAlpha);
    ET_SendEvent(labelId, &ETRenderNode::ET_setAlphaMultiplier, newAlpha);
}

void UIButton::onDisabled(bool flag) {
    if(flag) {
        ETNode<ETUIInteractionBox>::disconnect();
    } else {
        ETNode<ETUIInteractionBox>::connect(getEntityId());
    }
}