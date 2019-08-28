#include "UIEventTests.hpp"

#include <UI/ETUIInterfaces.hpp>
#include <UI/UISurfaceEventHandler.hpp>

namespace {

const char* MAIN_VIEW = "UI/MainView/Root.json";
const char* OPTIONS_VIEW = "UI/OptionsView/Root.json";
const char* EXIT_VIEW = "UI/ExitView/Root.json";
const char* GAME_VIEW = "UI/GameView/Root.json";
const char* PAUSE_VIEW = "UI/PauseView/Root.json";
const char* END_GAME_VIEW = "UI/EndGameView/Root.json";

void CheckExpectedView(const char* expectedView) {
    EntityId activeViewId;
    ET_SendEventReturn(activeViewId, &ETUIViewStack::ET_getActiveViewId);

    ASSERT_NE(activeViewId, InvalidEntityId);
    std::string activeViewName;
    ET_SendEventReturn(activeViewName, activeViewId, &ETEntity::ET_getName);
    ASSERT_STREQ(expectedView, activeViewName.c_str());
}

} // namespace

TEST_F(UIEventTests, CheckBackButtonOnOptionsView) {
    EntityId firstViewId;
    ET_SendEventReturn(firstViewId, &ETUIViewStack::ET_pushView, MAIN_VIEW);

    EntityId secondViewId;
    ET_SendEventReturn(secondViewId, &ETUIViewStack::ET_pushView, OPTIONS_VIEW);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    CheckExpectedView(MAIN_VIEW);
}

TEST_F(UIEventTests, CheckBackButtonOnMainView) {
    EntityId mainViewId;
    ET_SendEventReturn(mainViewId, &ETUIViewStack::ET_pushView, MAIN_VIEW);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    CheckExpectedView(EXIT_VIEW);
    
    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    CheckExpectedView(MAIN_VIEW);
}

TEST_F(UIEventTests, CheckBackButtonOnGameView) {
    EntityId gameViewId;
    ET_SendEventReturn(gameViewId, &ETUIViewStack::ET_pushView, GAME_VIEW);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    CheckExpectedView(PAUSE_VIEW);
    
    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    CheckExpectedView(GAME_VIEW);
}

TEST_F(UIEventTests, CheckBackButtonOnEndGameView) {
    EntityId gameViewId;
    ET_SendEventReturn(gameViewId, &ETUIViewStack::ET_pushView, END_GAME_VIEW);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    CheckExpectedView(MAIN_VIEW);
}

TEST_F(UIEventTests, CheckFastDoubleBack) {
    EntityId firstViewId;
    ET_SendEventReturn(firstViewId, &ETUIViewStack::ET_pushView, MAIN_VIEW);

    EntityId secondViewId;
    ET_SendEventReturn(secondViewId, &ETUIViewStack::ET_pushView, OPTIONS_VIEW);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);

    CheckExpectedView(MAIN_VIEW);
}

TEST_F(UIEventTests, CheckFastTripleBack) {
    EntityId firstViewId;
    ET_SendEventReturn(firstViewId, &ETUIViewStack::ET_pushView, MAIN_VIEW);

    EntityId secondViewId;
    ET_SendEventReturn(secondViewId, &ETUIViewStack::ET_pushView, OPTIONS_VIEW);

    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
    ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);

    CheckExpectedView(EXIT_VIEW);
}
