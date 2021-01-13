#include "Game/ViewScripts/EventSequence.hpp"
#include "Core/ETLogger.hpp"
#include "Entity/ETEntity.hpp"

#include <cassert>

EventSequence::EventSequence() {
}

EventSequence::~EventSequence() {
}

void EventSequence::init(EntityId newTriggerEntId) {
    triggerId = newTriggerEntId;
    ETNode<ETUIAnimationSequenceEvent>::connect(triggerId);
}

void EventSequence::addEvent(EntityId targetId, EAnimSequenceType animType) {
    assert(targetId.isValid() && "Invalid target id");

    Event newEvent;
    newEvent.animType = animType;
    newEvent.targetId = targetId;
    newEvent.afterCall = nullptr;
    pendingEvents.push_back(newEvent);
}

void EventSequence::addEventWithAfterCall(EntityId targetId, EAnimSequenceType animType, std::function<void(EntityId)> afterCall) {
   assert(targetId.isValid() && "Invalid target id");

    Event newEvent;
    newEvent.animType = animType;
    newEvent.targetId = targetId;
    newEvent.afterCall = afterCall;
    pendingEvents.push_back(newEvent);
}

void EventSequence::start() {
    startNextEvent();
}

void EventSequence::ET_onAnimationPlayed(EntityId targetId, EAnimSequenceType animType) {
    assert(!pendingEvents.empty() && "Pending events queue is empty");
    auto& event = pendingEvents.front();

    assert(event.targetId == targetId && "Invalid target id");

    if(event.afterCall) {
        event.afterCall(event.targetId);
    }

    pendingEvents.erase(pendingEvents.begin());
    startNextEvent();
}

void EventSequence::forceFinish() {
}

void EventSequence::startNextEvent() {
    while(!pendingEvents.empty()) {
        auto& event = pendingEvents.front();
        if(!UI::PlayAnimation(event.targetId, event.animType, triggerId)) {
            LogWarning("[EventSequence::startNextEvent] Can't start anim on target: '%s'", EntityUtils::GetEntityName(
                event.targetId));
            if(event.afterCall) {
               event.afterCall(event.targetId);
            }
            pendingEvents.erase(pendingEvents.begin());
        } else {
            break;
        }
    }
}