#ifndef __EVENT_SEQUENCE_HPP__
#define __EVENT_SEQUENCE_HPP__

#include "UI/ETUIAnimation.hpp"
#include "UI/ETUITimer.hpp"

class EventSequence : public ETNode<ETUIAnimationSequenceEvent>,
    public ETNode<ETUITimerEvents> {
public:

    struct Event {
    public:

        Event() : startDelay(0.f),
            animType(EAnimSequenceType::Appear) {}

    public:

        EntityId targetId;
        float startDelay;
        EAnimSequenceType animType;
        std::function<void(void)> onStartCallback;
        std::function<void(void)> onEndCallback;
    };

public:

    EventSequence();
    ~EventSequence();

    void init(EntityId newTriggerEntId);
    void deinit();

    void addEvent(const EventSequence::Event& newEvent);

    void start();
    void forceFinish();

    // ETUIAnimationSequenceEvent
    void ET_onAnimationPlayed(EntityId targetId, EAnimSequenceType animType) override;

    // ETUITimerEvents
    void ET_onUITick(float dt) override;

private:

    void startNextEvent();

private:

    EntityId triggerId;
    std::vector<Event> pendingEvents;
    Event* waitDelayEvent;
    bool isPlaying;
    bool isTicking;
};

#endif /* __EVENT_SEQUENCE_HPP__ */