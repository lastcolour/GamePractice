#include "UIViewSwitcherTests.hpp"
#include "TestUtils/UITestUtils.hpp"

#include <UI/UIViewSwitcher.hpp>
#include <UI/Logics/UIBox.hpp>
#include <Entity/Entity.hpp>

TEST_F(UISwitcherTests, CheckViewSwitch) {

    UIBox* firstView = createObjectAndLogic<UIBox>();
    ASSERT_TRUE(firstView->init());

    UIBox* secondView = createObjectAndLogic<UIBox>();
    ASSERT_TRUE(secondView->init());

    UIViewSwitcher switcher;
    ASSERT_TRUE(switcher.init());

    switcher.ET_swtichView(firstView->getEntityId(), secondView->getEntityId());

    {
        bool isVisible = true;
        ET_SendEventReturn(isVisible, firstView->getEntityId(), &ETUIVisibleElement::ET_isVisible);
        EXPECT_FALSE(isVisible);
    }

    float switchDuration = switcher.ET_getTotalSwitchDuration();
    switchDuration = 0.1f;
    switcher.ET_onAppTick(switchDuration);

    {
        bool isVisible = true;
        ET_SendEventReturn(isVisible, secondView->getEntityId(), &ETUIVisibleElement::ET_isVisible);
        EXPECT_FALSE(isVisible);
    }
}