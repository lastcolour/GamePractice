#include "GameDoubleTapTests.hpp"
#include "Game/Logics/GameBoardElemLogic.hpp"
#include "Game/Logics/DoubleTapDetection.hpp"
#include "Platform/ETSurface.hpp"

TEST_F(GameDoubleTapTests, CheckTapCount) {

    auto entity = createVoidObject();
    auto elemLogic = entity->addCustomLogic<GameBoardElemLogic>();
    elemLogic->ET_setType(EBoardElemType::Bomb);

    DoubleTapDetection tapDetector;

    {
        TouchEvent event;
        event.actionType = EActionType::Press;
        tapDetector.onTouchEvent(entity->getEntityId(), event);
    }

    {
        TouchEvent event;
        event.actionType = EActionType::Release;
        tapDetector.onTouchEvent(entity->getEntityId(), event);
    }

    auto tapInfo = tapDetector.getTapInfo();

    EXPECT_EQ(tapInfo.targetId, entity->getEntityId());
    EXPECT_EQ(tapInfo.tapCount, 1);

    {
        TouchEvent event;
        event.actionType = EActionType::Press;
        tapDetector.onTouchEvent(entity->getEntityId(), event);
    }

    {
        TouchEvent event;
        event.actionType = EActionType::Release;
        tapDetector.onTouchEvent(entity->getEntityId(), event);
    }

    tapInfo = tapDetector.getTapInfo();

    EXPECT_EQ(tapInfo.targetId, entity->getEntityId());
    EXPECT_EQ(tapInfo.tapCount, 2);
}

TEST_F(GameDoubleTapTests, CheckTapAnotherElem) {

    auto firstEnt = createVoidObject();
    {
        auto elemLogic = firstEnt->addCustomLogic<GameBoardElemLogic>();
        elemLogic->ET_setType(EBoardElemType::Bomb);
    }

    auto secondEnt = createVoidObject();
    {
        auto elemLogic = secondEnt->addCustomLogic<GameBoardElemLogic>();
        elemLogic->ET_setType(EBoardElemType::Bomb);
    }

    DoubleTapDetection tapDetector;

    {
        TouchEvent event;
        event.actionType = EActionType::Press;
        tapDetector.onTouchEvent(firstEnt->getEntityId(), event);

        event.actionType = EActionType::Move;
        tapDetector.onTouchEvent(firstEnt->getEntityId(), event);

        event.actionType = EActionType::Release;
        tapDetector.onTouchEvent(firstEnt->getEntityId(), event);
    }

    auto tapInfo = tapDetector.getTapInfo();

    EXPECT_EQ(tapInfo.targetId, firstEnt->getEntityId());
    EXPECT_EQ(tapInfo.tapCount, 1);

    {
        TouchEvent event;
        event.actionType = EActionType::Press;
        tapDetector.onTouchEvent(secondEnt->getEntityId(), event);

        event.actionType = EActionType::Move;
        tapDetector.onTouchEvent(secondEnt->getEntityId(), event);

        event.actionType = EActionType::Release;
        tapDetector.onTouchEvent(secondEnt->getEntityId(), event);
    }

    tapInfo = tapDetector.getTapInfo();
    EXPECT_EQ(tapInfo.targetId, secondEnt->getEntityId());
    EXPECT_EQ(tapInfo.tapCount, 1);
}

TEST_F(GameDoubleTapTests, CheckTapExpire) {

    auto entity = createVoidObject();
    auto elemLogic = entity->addCustomLogic<GameBoardElemLogic>();
    elemLogic->ET_setType(EBoardElemType::Bomb);


    auto startT = TimePoint::GetNowTime();
    DoubleTapDetection tapDetector;

    {
        TouchEvent event;
        event.eventT = startT;
    
        event.actionType = EActionType::Press;
        tapDetector.onTouchEvent(entity->getEntityId(), event);

        event.actionType = EActionType::Release;
        tapDetector.onTouchEvent(entity->getEntityId(), event);
    }

    auto tapInfo = tapDetector.getTapInfo();

    EXPECT_EQ(tapInfo.targetId, entity->getEntityId());
    EXPECT_EQ(tapInfo.tapCount, 1);

    {
        TouchEvent event;
        event.eventT = startT;
        event.eventT.addSecDelta(0.5f);

        event.actionType = EActionType::Press;
        tapDetector.onTouchEvent(entity->getEntityId(), event);

        event.actionType = EActionType::Release;
        tapDetector.onTouchEvent(entity->getEntityId(), event);
    }

    tapInfo = tapDetector.getTapInfo();

    EXPECT_EQ(tapInfo.targetId, entity->getEntityId());
    EXPECT_EQ(tapInfo.tapCount, 1);
}

TEST_F(GameDoubleTapTests, CheckLongRelease) {

    auto entity = createVoidObject();
    auto elemLogic = entity->addCustomLogic<GameBoardElemLogic>();
    elemLogic->ET_setType(EBoardElemType::Bomb);


    auto startT = TimePoint::GetNowTime();
    DoubleTapDetection tapDetector;

    {
        TouchEvent event;
        event.eventT = startT;
    
        event.actionType = EActionType::Press;
        tapDetector.onTouchEvent(entity->getEntityId(), event);

        event.eventT.addSecDelta(0.5f);
        event.actionType = EActionType::Release;
        tapDetector.onTouchEvent(entity->getEntityId(), event);
    }

    auto tapInfo = tapDetector.getTapInfo();
    EXPECT_EQ(tapInfo.targetId, InvalidEntityId);
    EXPECT_EQ(tapInfo.tapCount, 0);
}