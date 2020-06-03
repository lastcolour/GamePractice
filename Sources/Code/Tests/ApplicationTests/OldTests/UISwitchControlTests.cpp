#include "UISwitchControlTests.hpp"
#include "UI/Logics/UISwitchControl.hpp"
#include "UI/Logics/UIBox.hpp"
#include "Core/JSONNode.hpp"

#include <algorithm>

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
    auto gameObj = createVoidObject();

    UIBox* uiBox = nullptr;
    {
        uiBox = new UIBox();
        gameObj->addCustomLogic(std::unique_ptr<EntityLogic>(uiBox));

        std::string serStr = "{\"label\":{\"text\":\"TestText\"}}";
        JSONNode node = JSONNode::ParseString(serStr.c_str());
        ASSERT_TRUE(node);
        //ASSERT_TRUE(uiBox->serialize(node));
        ASSERT_TRUE(uiBox->init());
    }

    UISwitchControl* uiSwitchControl = nullptr;
    {
        uiSwitchControl = new UISwitchControl();
        gameObj->addCustomLogic(std::unique_ptr<EntityLogic>(uiSwitchControl));
        ASSERT_TRUE(uiSwitchControl->init());
    }

    const char* fistLabelText = "On";
    const char* secondLabelText = "Off";
    if(uiSwitchControl->ET_isEnabled()) {
        std::swap(fistLabelText, secondLabelText);
    }

    uiSwitchControl->ET_onPress();

    {
        const char* cLabelStr = uiBox->ET_getLabelText();
        EXPECT_STREQ(cLabelStr, fistLabelText);
    }

    uiSwitchControl->ET_onPress();

    {
        const char* cLabelStr = uiBox->ET_getLabelText();
        EXPECT_STREQ(cLabelStr,  secondLabelText);
    }
}