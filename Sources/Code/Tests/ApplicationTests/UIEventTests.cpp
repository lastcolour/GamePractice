#include "UIEventTests.hpp"

#include <UI/ETUIInterfaces.hpp>
#include <UI/UISurfaceEventHandler.hpp>
#include <Game/ETGameInterfaces.hpp>
#include "Core/ETTimer.hpp"

#include "TestUtils/UITestUtils.hpp"
#include "TestUtils/GameTestUtils.hpp"

namespace {

const char* MAIN_VIEW = "UI/MainView/Root.json";
const char* OPTIONS_VIEW = "UI/OptionsView/Root.json";
const char* EXIT_VIEW = "UI/ExitView/Root.json";
const char* GAME_VIEW = "UI/GameView/Root.json";
const char* PAUSE_VIEW = "UI/PauseView/Root.json";
const char* END_GAME_VIEW = "UI/EndGameView/Root.json";

const char* RESTART_GAME_EVENT = "Pause_OnRestartButton";
const char* START_GAME_EVENT = "Main_OnStartButton";

} // namespace

void UIEventTests::TearDown() {
    ET_SendEvent(&ETGameStateManager::ET_interruptGame);
    ET_SendEvent(&ETUIViewStack::ET_forceReset);
    ConsoleAppTests::TearDown();
}

TEST_F(UIEventTests, CheckBackButtonOnOptionsView) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, MAIN_VIEW);

    ET_SendEvent(&ETUIViewStack::ET_pushView, OPTIONS_VIEW);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    CheckExpectedView(MAIN_VIEW);
}

TEST_F(UIEventTests, CheckBackButtonOnMainView) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, MAIN_VIEW);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    CheckExpectedView(EXIT_VIEW);
    
    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    CheckExpectedView(MAIN_VIEW);
}

TEST_F(UIEventTests, CheckBackButtonOnGameView) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, GAME_VIEW);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    CheckExpectedView(PAUSE_VIEW);
    
    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    CheckExpectedView(GAME_VIEW);
}

TEST_F(UIEventTests, CheckBackButtonOnEndGameView) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, END_GAME_VIEW);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    CheckExpectedView(MAIN_VIEW);
}

TEST_F(UIEventTests, CheckFastDoubleBack) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, MAIN_VIEW);

    ET_SendEvent(&ETUIViewStack::ET_pushView, OPTIONS_VIEW);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);

    CheckExpectedView(MAIN_VIEW);
}

TEST_F(UIEventTests, CheckFastBackFromOptions) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, MAIN_VIEW);

    ET_SendEvent(&ETUIViewStack::ET_pushView, OPTIONS_VIEW);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);

    CheckExpectedView(MAIN_VIEW);
}

TEST_F(UIEventTests, CheckTripleBackOnMain) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, MAIN_VIEW);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);

    CheckExpectedView(EXIT_VIEW);
}

TEST_F(UIEventTests, CheckGameTimerStopOnPause) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, GAME_VIEW);
    WaitPreGameEnd();

    float oldRemainingTime = -1.f;
    ET_SendEventReturn(oldRemainingTime, &ETGameEndTimer::ET_getRemainingTime);
    ASSERT_GT(oldRemainingTime, 0.f);

    {
        float tickDuration = 0.5f;
        ET_SendEvent(&ETAppTimerEvents::ET_onAppTick, tickDuration);

        float newRemainingTime = -1.f;
        ET_SendEventReturn(newRemainingTime, &ETGameEndTimer::ET_getRemainingTime);
        ASSERT_FLOAT_EQ(newRemainingTime, oldRemainingTime - tickDuration);
    }

    ET_SendEventReturn(oldRemainingTime, &ETGameEndTimer::ET_getRemainingTime);
    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);

    {
        float tickDuration = 0.5f;
        ET_SendEvent(&ETAppTimerEvents::ET_onAppTick, tickDuration);

        float newRemainingTime = -1.f;
        ET_SendEventReturn(newRemainingTime, &ETGameEndTimer::ET_getRemainingTime);
        ASSERT_FLOAT_EQ(newRemainingTime, oldRemainingTime);
    }
}

TEST_F(UIEventTests, CheckGameEndTimerResumeAfterBack) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, GAME_VIEW);
    WaitPreGameEnd();

    float oldRemainingTime = -1.f;
    ET_SendEventReturn(oldRemainingTime, &ETGameEndTimer::ET_getRemainingTime);
    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);

    ASSERT_GT(oldRemainingTime, 0.f);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);

    float tickDuration = 0.5f;
    ET_SendEvent(&ETAppTimerEvents::ET_onAppTick, 0.f);
    ET_SendEvent(&ETAppTimerEvents::ET_onAppTick, tickDuration);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);

    ET_SendEvent(&ETAppTimerEvents::ET_onAppTick, tickDuration);

    float newRemainingTime = -1.f;
    ET_SendEventReturn(newRemainingTime, &ETGameEndTimer::ET_getRemainingTime);

    ASSERT_FLOAT_EQ(newRemainingTime, oldRemainingTime - tickDuration);
}

TEST_F(UIEventTests, CheckGameRestart) {
    ET_SendEvent(&ETUIEventManager::ET_onEvent, START_GAME_EVENT);

    float tickDuration = 1.f;
    ET_SendEvent(&ETAppTimerEvents::ET_onAppTick, tickDuration);

    float oldPlayTime = -1.f;
    ET_SendEventReturn(oldPlayTime, &ETGameEndTimer::ET_getRemainingTime);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);

    ET_SendEvent(&ETUIEventManager::ET_onEvent, RESTART_GAME_EVENT);

    ET_SendEvent(&ETAppTimerEvents::ET_onAppTick, tickDuration);

    float newPlayTime = -1.f;
    ET_SendEventReturn(newPlayTime, &ETGameEndTimer::ET_getRemainingTime);

    ASSERT_FLOAT_EQ(oldPlayTime, newPlayTime);
}

TEST_F(UIEventTests, CheckGamePauseWhenSurfaceHidden) {
    ET_SendEvent(&ETUIEventManager::ET_onEvent, START_GAME_EVENT);

    ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceHidden);

    bool isGamePaused = false;
    ET_SendEventReturn(isGamePaused, &ETGameStateManager::ET_isGamePaused);

    EXPECT_TRUE(isGamePaused);

    ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceShown);

    CheckExpectedView(PAUSE_VIEW);
}