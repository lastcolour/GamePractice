#include "UIViewStackTests.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "TestUtils/UITestUtils.hpp"
#include "ETApplicationInterfaces.hpp"

namespace {

const char* TEST_VIEW_1 = "UI/PauseView/Root.json";
const char* TEST_VIEW_2 = "UI/ExitView/Root.json";
const char* TEST_VIEW_3 = "UI/AboutView/Root.json";

}; // namespace

TEST_F(UIViewStackTests, CheckPushPushPop) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);
    ET_SendEvent(&ETUIViewStack::ET_popView);

    CheckExpectedView(TEST_VIEW_1);
}

TEST_F(UIViewStackTests, CheckPushPushPopPush) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);
    ET_SendEvent(&ETUIViewStack::ET_popView);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_3);

    CheckExpectedView(TEST_VIEW_3);
}

TEST_F(UIViewStackTests, CheckPushPushPopPushTheSame) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);
    ET_SendEvent(&ETUIViewStack::ET_popView);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);

    CheckExpectedView(TEST_VIEW_2);
}

TEST_F(UIViewStackTests, CheckTriplePush) {
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_1);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_2);
    ET_SendEvent(&ETUIViewStack::ET_pushView, TEST_VIEW_3);

    CheckExpectedView(TEST_VIEW_2);

    float switchTime = 0.f;
    ET_SendEventReturn(switchTime, &ETUIViewSwitcher::ET_getSwitchDuration);
    ET_SendEvent(&ETTimerEvents::ET_onTick, switchTime + 0.1f);

    CheckExpectedView(TEST_VIEW_3);
}