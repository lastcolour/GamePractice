#ifndef __EVENT_SEQUENCE_HPP__
#define __EVENT_SEQUENCE_HPP__

#include "Core/ETPrimitives.hpp"
#include "UI/ETUIAnimation.hpp"

#include <functional>

class EventSequence : public ETNode<ETUIAnimationSequenceEvent> {
public:

    EventSequence();
    ~EventSequence();

    void init(EntityId newTriggerEntId);
    void deinit();

    void addEvent(EntityId entityId, EAnimSequenceType animType);
    void addEventWithAfterCall(EntityId entityId, EAnimSequenceType animType, std::function<void(EntityId)> afterCall);

    void start();
    void forceFinish();

    // ETUIAnimationSequenceEvent
    void ET_onAnimationPlayed(EntityId targetId, EAnimSequenceType animType) override;

private:

    struct Event {
        EntityId targetId;
        EAnimSequenceType animType;
        std::function<void(EntityId)> afterCall;
    };

private:

    void startNextEvent();

private:

    EntityId triggerId;
    std::vector<Event> pendingEvents;
};

#endif /* __EVENT_SEQUENCE_HPP__ */