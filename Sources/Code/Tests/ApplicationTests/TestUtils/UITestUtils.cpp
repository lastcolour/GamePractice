#include "TestUtils/UITestUtils.hpp"
#include "UI/ETUIView.hpp"
#include "Entity/ETEntity.hpp"
#include "UI/ETUIButton.hpp"
#include "Logics/UIBox.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/ETTimer.hpp"

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
    ET_SendEventReturn(switchTime, &ETUIViewSwitcher::ET_getTotalSwitchDuration);
    ET_SendEvent(&ETAppTimerEvents::ET_onAppTick, switchTime + 0.000001f);
}

void WaitButtonReleaseEvent() {
    float pressDuration = 0.f;
    ET_SendEventReturn(pressDuration, &ETUIButton::ET_getPressDuration);
    ET_SendEvent(&ETAppTimerEvents::ET_onAppTick, pressDuration + 0.000001f);
}

void CheckUIBoxSizeAndCenter(const UIBox* uiBox, const Vec2i& expectedSize, const Vec2i& expectedCenter) {
    ASSERT_TRUE(uiBox);

    auto& box = uiBox->ET_getBox();

    auto size = box.getSize();
    EXPECT_EQ(size.x, expectedSize.x);
    EXPECT_EQ(size.y, expectedSize.y);

    auto center = box.getCenter();
    EXPECT_EQ(center.x, expectedCenter.x);
    EXPECT_EQ(center.y, expectedCenter.y);
}