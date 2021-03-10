#include "Game/ViewScripts/EventSequence.hpp"
#include "Core/ETLogger.hpp"
#include "Entity/ETEntity.hpp"
#include "UI/ETUIBox.hpp"

#include <cassert>

namespace {

bool PlayAnimtionSafe(EntityId targetId, EAnimSequenceType animType, EntityId triggerId) {
    if(!targetId.isValid()) {
        return false;
    }
    if(UI::PlayAnimation(targetId, animType, triggerId)) {
        return true;
    }

    LogWarning("[EventSequence::PlayAnimtionSafe] Can't start anim on target: '%s'",
        EntityUtils::GetEntityName(targetId));

    if(animType == EAnimSequenceType::Appear) {
        ET_SendEvent(targetId, &ETUIElement::ET_show);
    } else if(animType == EAnimSequenceType::Disappear) {
        ET_SendEvent(targetId, &ETUIElement::ET_hide);
    }
    return false;
}

} // namespace

EventSequence::EventSequence() :
    waitDelayEvent(nullptr),
    isPlaying(false),
    isTicking(false) {
}

EventSequence::~EventSequence() {
}

void EventSequence::init(EntityId newTriggerEntId) {
    triggerId = newTriggerEntId;
    ETNode<ETUIAnimationSequenceEvent>::connect(triggerId);
}

void EventSequence::addEvent(const EventSequence::Event& newEvent) {
    assert(triggerId.isValid() && "Event sequence not initted");

    bool isVoidEvent = true;
    if(newEvent.targetId.isValid()) {
        isVoidEvent = false;
    }
    if(newEvent.onEndCallback) {
        isVoidEvent = false;
    }
    if(newEvent.onStartCallback) {
        isVoidEvent = false;
    }
    if(isVoidEvent) {
        assert(false && "Can't add void event to the sequence");
        return;
    }

    pendingEvents.push_back(newEvent);
}

void EventSequence::start() {
    if(isPlaying) {
        return;
    }
    isPlaying = true;
    startNextEvent();
}

void EventSequence::ET_onAnimationPlayed(EntityId targetId, EAnimSequenceType animType) {
    assert(!pendingEvents.empty() && "Pending events queue is empty");
    auto& event = pendingEvents.front();

    assert(event.targetId == targetId && "Invalid target id");

    if(event.onEndCallback) {
        event.onEndCallback();
    }

    pendingEvents.erase(pendingEvents.begin());
    startNextEvent();
}

void EventSequence::ET_onUITick(float dt) {
    assert(!pendingEvents.empty() && "no pending events");
    if(!waitDelayEvent) {
        return;
    }
    assert(waitDelayEvent == &pendingEvents.front() && "invalid delayed event");
    waitDelayEvent->startDelay -= dt;
    if(waitDelayEvent->startDelay <= 0.f) {
        waitDelayEvent = nullptr;
        startNextEvent();
    }
}

void EventSequence::forceFinish() {
    if(!isPlaying) {
        return;
    }
    while(!pendingEvents.empty()) {
        ET_SendEvent(&ETUITimerEvents::ET_onUITick, 32.f);
    }
}

void EventSequence::startNextEvent() {
    while(!pendingEvents.empty()) {
        auto& event = pendingEvents.front();
        if(event.startDelay > 0.f) {
            waitDelayEvent = &event;
            if(!isTicking) {
                isTicking = true;
                ETNode<ETUITimerEvents>::connect(triggerId);
            }
            break;
        }
        if(event.onStartCallback) {
            event.onStartCallback();
        }
        if(!PlayAnimtionSafe(event.targetId, event.animType, triggerId)) {
            if(event.onEndCallback) {
                event.onEndCallback();
            }
            pendingEvents.erase(pendingEvents.begin());
        } else {
            break;
        }
    }

    if(pendingEvents.empty()) {
        isPlaying = false;
        if(isTicking) {
            ETNode<ETUITimerEvents>::disconnect();
        }
        isTicking = false;
    }
}