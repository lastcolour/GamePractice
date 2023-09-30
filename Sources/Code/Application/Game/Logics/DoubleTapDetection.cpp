#include "Game/Logics/DoubleTapDetection.hpp"
#include "Game/Logics/GameBoardUtils.hpp"

#include <cassert>

namespace {

const float TAP_DELTA = 0.2f;
const float PRESS_RELEASE_DELTA = 0.3f;

} // namespace

DoubleTapDetection::DoubleTapDetection() :
    tapCount(0) {
}

DoubleTapDetection::~DoubleTapDetection() {
}

void DoubleTapDetection::onTouchEvent(EntityId targetId, const TouchEvent& event) {
    switch(event.actionType) {
        case EActionType::Press: {
            if(!targetId.isValid()) {
                reset();
                return;
            }
            auto elemType = GameUtils::GetElemType(targetId);
            if(elemType == EBoardElemType::Star || !GameUtils::IsTriggerType(elemType)) {
                reset();
                return;
            }
            if(targetId != prevTap.targetId) {
                reset();
            }
            if(event.eventT.getSecDeltaWith(prevTap.touch.eventT) > TAP_DELTA) {
                reset();
            }
            prevTap = {event, targetId};
            break;
        }
        case EActionType::Move: {
            if(targetId != prevTap.targetId) {
                reset();
            }
            break;
        }
        case EActionType::Release: {
            if(!prevTap.targetId.isValid()) {
                return;
            }
            if(prevTap.targetId != targetId) {
                return; 
            }
            if(event.eventT.getSecDeltaWith(prevTap.touch.eventT) > PRESS_RELEASE_DELTA) {
                reset();
            } else {
                prevTap.touch = event;
                ++tapCount;
            }
            break;
        }
        case EActionType::ReleaseAndIgnore: {
            reset();
            break;
        }
        default: {
            assert(false && "Invalid action type");
        }
    }
}

DoubleTapDetection::TapInfo DoubleTapDetection::getTapInfo() const {
    return {prevTap.targetId, tapCount};
}

void DoubleTapDetection::reset() {
    prevTap.targetId = InvalidEntityId;
    prevTap.touch = TouchEvent();
    tapCount = 0;
}