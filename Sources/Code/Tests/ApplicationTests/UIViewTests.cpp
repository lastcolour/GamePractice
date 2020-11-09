#include "UIViewTests.hpp"
#include "UI/ETUIView.hpp"
#include "UI/ETUITimer.hpp"
#include "UI/ETUIViewCache.hpp"
#include "Core/ETTasks.hpp"

namespace {

void WaitViewLoaded() {
    ET_SendEvent(&ETEntitiesUpdateTask::ET_updateEntities);
    ET_PollAllEvents<ETUIViewCache>();
}

void WaitAppearAnimation() {
    ET_SendEvent(&ETUITimerEvents::ET_onUITick, 10.f);
}

bool GetIsInFocus(EntityId viewId) {
    bool hasFocus = false;
    ET_SendEventReturn(hasFocus, viewId, &ETUIView::ET_getFocus);
    return hasFocus;
}

} // namespace

TEST_F(UIViewTests, CheckViewGotFocus) {
    {
        EntityId viewId;
        ET_SendEvent(viewId, &ETUIViewManager::ET_getActiveViewId);
        EXPECT_FALSE(viewId.isValid());
    }
    {
        bool result = false;
        ET_SendEventReturn(result, &ETUIViewManager::ET_openView, UIViewType::Background);
        EXPECT_TRUE(result);
        WaitViewLoaded();
    }
    EntityId backgroundViewId;
    {
        ET_SendEventReturn(backgroundViewId, &ETUIViewManager::ET_getActiveViewId);
        EXPECT_TRUE(backgroundViewId.isValid());
        EXPECT_TRUE(GetIsInFocus(backgroundViewId));
    }
    {
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Background);
        EXPECT_FALSE(GetIsInFocus(backgroundViewId));
    }
}

TEST_F(UIViewTests, CheckOnlyOneViewHasFocus) {
    EntityId backgroundViewId;
    {
        bool result = false;
        ET_SendEventReturn(result, &ETUIViewManager::ET_openView, UIViewType::Background);
        EXPECT_TRUE(result);

        ET_SendEventReturn(backgroundViewId, &ETUIViewManager::ET_getActiveViewId);
        EXPECT_TRUE(backgroundViewId.isValid());
    }

    EXPECT_TRUE(GetIsInFocus(backgroundViewId));

    {
        bool result = false;
        ET_SendEventReturn(result, &ETUIViewManager::ET_openView, UIViewType::Main);
        EXPECT_TRUE(result);
    }

    EXPECT_FALSE(GetIsInFocus(backgroundViewId));

    WaitViewLoaded();

    EntityId mainViewId;

    {
        ET_SendEventReturn(mainViewId, &ETUIViewManager::ET_getActiveViewId);
        EXPECT_TRUE(mainViewId.isValid());
    }

    EXPECT_FALSE(GetIsInFocus(mainViewId));
    EXPECT_FALSE(GetIsInFocus(backgroundViewId));

    WaitAppearAnimation();

    EXPECT_TRUE(GetIsInFocus(mainViewId));
    EXPECT_FALSE(GetIsInFocus(backgroundViewId));

    {
        ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Main);
    }

    EXPECT_FALSE(GetIsInFocus(mainViewId));
    EXPECT_FALSE(GetIsInFocus(backgroundViewId));

    WaitAppearAnimation();

    EXPECT_TRUE(GetIsInFocus(backgroundViewId));

    ET_SendEvent(&ETUIViewManager::ET_closeView, UIViewType::Background);
}