#include "UITestUtils.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "Entity/ETEntityInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "UI/Logics/UIBaseBox.hpp"

#include <gtest/gtest.h>

void CheckExpectedView(const char* expectedView) {
    EntityId activeViewId;
    ET_SendEventReturn(activeViewId, &ETUIViewStack::ET_getActiveViewId);

    ASSERT_NE(activeViewId, InvalidEntityId);
    std::string activeViewName;
    ET_SendEventReturn(activeViewName, activeViewId, &ETEntity::ET_getName);
    ASSERT_STREQ(expectedView, activeViewName.c_str());
}

void WaitViewSwitchEnd() {
    float switchTime = 0.f;
    ET_SendEventReturn(switchTime, &ETUIViewSwitcher::ET_getSwitchDuration);
    ET_SendEvent(&ETTimerEvents::ET_onTick, switchTime + 0.000001f);
}

void WaitButtonReleaseEvent() {
    float pressDuration = 0.f;
    ET_SendEventReturn(pressDuration, &ETUIButton::ET_getPressDuration);
    ET_SendEvent(&ETTimerEvents::ET_onTick, pressDuration + 0.000001f);
}

void CheckUIBoxSizeAndCenter(const UIBaseBox* uiBox, const Vec2i& expectedSize, const Vec2i& expectedCenter) {
    ASSERT_TRUE(uiBox);

    auto& box = uiBox->ET_getAabb2di();

    auto size = box.getSize();
    EXPECT_EQ(size.x, expectedSize.x);
    EXPECT_EQ(size.y, expectedSize.y);

    auto center = box.getCenter();
    EXPECT_EQ(center.x, expectedCenter.x);
    EXPECT_EQ(center.y, expectedCenter.y);
}