#include "UIEventTests.hpp"

#include <UI/ETUIInterfaces.hpp>
#include <UI/UISurfaceEventHandler.hpp>

#include "TestUtils/UITestUtils.hpp"

namespace {

const char* MAIN_VIEW = "UI/MainView/Root.json";
const char* OPTIONS_VIEW = "UI/OptionsView/Root.json";
const char* EXIT_VIEW = "UI/ExitView/Root.json";
const char* GAME_VIEW = "UI/GameView/Root.json";
const char* PAUSE_VIEW = "UI/PauseView/Root.json";
const char* END_GAME_VIEW = "UI/EndGameView/Root.json";

} // namespace

void UIEventTests::TearDown() {
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

TEST_F(UIEventTests, CheckFastTripleBack) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, MAIN_VIEW);

    ET_SendEvent(&ETUIViewStack::ET_pushView, OPTIONS_VIEW);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);

    CheckExpectedView(EXIT_VIEW);
}
