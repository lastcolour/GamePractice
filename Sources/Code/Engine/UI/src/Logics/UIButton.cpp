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

const float MOVE_AWAY_GRID_LEN = 7.f;
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
    auto dt = now.getSecElapsedFrom(pressTime);
    if(dt > TIME_OUT_SEC_DUR) {
        return true;
    }
    return false;
}

} // namespace

void UIButton::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<UIEvent::EventType>("UIEventType")) {
        enumInfo->addValues<UIEvent::EventType>({
            {"None", UIEvent::EventType::None},
            {"OnMainViewStartGame", UIEvent::EventType::OnMainViewStartGame},
            {"OnGameEndViewExit", UIEvent::EventType::OnGameEndViewExit},
            {"OnBackButton", UIEvent::EventType::OnBackButton},
            {"OnGameGameEnd", UIEvent::EventType::OnGameGameEnd},
            {"OnSurfaceHidden", UIEvent::EventType::OnSurfaceHidden},
            {"OnSurfaceShown", UIEvent::EventType::OnSurfaceShown},
            {"OnPauseViewResume", UIEvent::EventType::OnPauseViewResume},
            {"OnPauseViewRestart", UIEvent::EventType::OnPauseViewRestart},
            {"OnPauseViewExit", UIEvent::EventType::OnPauseViewExit},
            {"OnLevelsStartLevel", UIEvent::EventType::OnLevelsStartLevel}
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
    eventType(UIEvent::EventType::None) {
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
    EInputEventResult res = EInputEventResult::Ignore;
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
        default: {
            assert(false && "Invalid input event");
            break;
        }
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
    auto hitBox = ET_getHitBox();
    if(!hitBox.isInside(pt)) {
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
        UIEvent buttonEvent{getEntityId(), eventType};
        ET_SendEvent(&ETUIViewScript::ET_onEvent, buttonEvent);
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
    EntityId activeBtId;
    ET_SendEventReturn(activeBtId, &ETUIButtonEventManager::ET_getActiveButton);
    assert(activeBtId == getEntityId());

    ET_SendEvent(&ETUIButtonEventManager::ET_setActiveButton, InvalidEntityId);

    UIEvent buttonEvent{getEntityId(), eventType};
    ET_SendEvent(&ETUIViewScript::ET_onEvent, buttonEvent);
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
    ET_SendEvent(labelId, &ETRenderNode::ET_setVisibilityMultiplier, !flag);
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