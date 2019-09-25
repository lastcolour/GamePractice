#include "UISwitchControlTests.hpp"
#include "UI/Logics/UISwitchControl.hpp"

TEST_F(UISwitchControlTests, CheckConnections) {
    auto uiSwitchControl = createObjectAndLogic<UISwitchControl>();
    ASSERT_TRUE(uiSwitchControl->init());

    auto entId = uiSwitchControl->getEntityId();

    EXPECT_TRUE(ET_IsExistNode<ETUISwitchControl>(entId));
    EXPECT_TRUE(ET_IsExistNode<ETUIInteractionBox>(entId));
}

TEST_F(UISwitchControlTests, CheckSwitchOnOff) {
    auto uiSwitchControl = createObjectAndLogic<UISwitchControl>();
    ASSERT_TRUE(uiSwitchControl->init());

    bool isEnabled = uiSwitchControl->ET_isEnabled();
    EXPECT_FALSE(isEnabled);

    uiSwitchControl->ET_onPress();

    isEnabled = uiSwitchControl->ET_isEnabled();
    EXPECT_TRUE(isEnabled);

    uiSwitchControl->ET_onPress();

    isEnabled = uiSwitchControl->ET_isEnabled();
    EXPECT_FALSE(isEnabled);
}

TEST_F(UISwitchControlTests, CheckSetLabel) {
    ASSERT_TRUE(false);
}