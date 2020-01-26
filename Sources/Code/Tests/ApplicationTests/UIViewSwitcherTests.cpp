#include "UIViewSwitcherTests.hpp"
#include "TestUtils/UITestUtils.hpp"

#include <UI/UIViewSwitcher.hpp>
#include <UI/Logics/UIBaseBox.hpp>
#include <Entity/Entity.hpp>

TEST_F(UISwitcherTests, CheckViewSwitch) {

    UIBaseBox* firstView = createObjectAndLogic<UIBaseBox>();
    ASSERT_TRUE(firstView->init());

    UIBaseBox* secondView = createObjectAndLogic<UIBaseBox>();
    ASSERT_TRUE(secondView->init());

    UIViewSwitcher switcher;
    ASSERT_TRUE(switcher.init());

    switcher.ET_swtichView(firstView->getEntityId(), secondView->getEntityId());

    {
        bool isVisible = true;
        ET_SendEventReturn(isVisible, firstView->getEntityId(), &ETUIBox::ET_isVisible);
        EXPECT_FALSE(isVisible);
    }

    float switchDuration = switcher.ET_getTotalSwitchDuration();
    switchDuration = 0.1f;
    switcher.ET_onTick(switchDuration);

    {
        bool isVisible = true;
        ET_SendEventReturn(isVisible, secondView->getEntityId(), &ETUIBox::ET_isVisible);
        EXPECT_FALSE(isVisible);
    }
}