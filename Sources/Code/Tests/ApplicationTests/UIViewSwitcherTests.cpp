#include "UIViewSwitcherTests.hpp"
#include "TestUtils/UITestUtils.hpp"

#include <UI/UIViewSwitcher.hpp>
#include <UI/Logics/UIBaseBox.hpp>
#include <Entity/Entity.hpp>

TEST_F(UISwitcherTests, CheckViewSwtichInit) {

    UIBaseBox* firstView = createObjectAndLogic<UIBaseBox>();
    ASSERT_TRUE(firstView->init());

    UIBaseBox* secondView = createObjectAndLogic<UIBaseBox>();
    ASSERT_TRUE(secondView->init());

    UIViewSwitcher switcher;
    ASSERT_TRUE(switcher.init());

    switcher.ET_swtichView(firstView->getEntityId(), secondView->getEntityId());
}