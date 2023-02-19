#include "GameEventSequenceTests.hpp"
#include "Game/ViewScripts/EventSequence.hpp"
#include "Logics/UIAnimationSequence.hpp"

TEST_F(GameEventSequenceTests, CheckForceFinish) {
    auto entity = createVoidObject();

    auto animSeq = entity->addCustomLogic<UIAnimationSequence>();
    {
        animSeq->ET_setType(EAnimSequenceType::Idle);
        UIAnimationFrame frame;
        frame.duration = 1.f;
        animSeq->ET_addAnimationFrame(frame);
    }

    EventSequence seq;
    int onStartCalled = 0;
    int onEndCalled = 0;

    seq.init(entity->getEntityId());
    {
        EventSequence::Event event;
        event.targetId = entity->getEntityId();
        event.animType = EAnimSequenceType::Idle;
        event.onStartCallback = [&onStartCalled]() {
            ++onStartCalled;
        };
        event.onEndCallback = [&onEndCalled]() {
            ++onEndCalled;
        };
        seq.addEvent(event);
    }

    seq.start();

    {
        bool playing = false;
        ET_SendEventReturn(playing, entity->getEntityId(), &ETUIAnimationSequence::ET_isPlaying);
        EXPECT_TRUE(playing);
    }

    ET_SendEvent(&ETUITimerEvents::ET_onUITick, 0.5f);

    seq.forceFinish();

    {
        bool playing = true;
        ET_SendEventReturn(playing, entity->getEntityId(), &ETUIAnimationSequence::ET_isPlaying);
        EXPECT_FALSE(playing);
    }

    EXPECT_EQ(onStartCalled, 1);
    EXPECT_EQ(onEndCalled, 1);
}