#include "UIMarginTests.hpp"
#include "UI/Logics/UIList.hpp"
#include "TestUtils/UITestUtils.hpp"

TEST_F(UIMarginTests, CheckLeftMargin) {
    auto leftBox = createObjectAndLogic<UIBaseBox>();
    {
        UIStyle style;
        style.size = Vec2(100.f);
        style.sizeInv = SizeInvariant::Pixel;
        leftBox->ET_setStyle(style);
        ASSERT_TRUE(leftBox->init());
    }
    auto rightBox = createObjectAndLogic<UIBaseBox>();
    {
        UIStyle style;
        style.size = Vec2(100.f);
        style.sizeInv = SizeInvariant::Pixel;
        style.margin.left = 10.f;
        rightBox->ET_setStyle(style);
        ASSERT_TRUE(rightBox->init());
    }
    {
        auto uiList = createObjectAndLogic<UIList>();
        uiList->ET_setType(UIListType::Horizontal);

        UIStyle style;
        style.xAlignType = XAlignType::Left;
        style.yAlignType = YAlignType::Bot;
        uiList->ET_setStyle(style);

        ASSERT_TRUE(uiList->init());
        ET_SendEvent(uiList->getEntityId(), &ETEntity::ET_addChild, leftBox->getEntityId());
        ET_SendEvent(uiList->getEntityId(), &ETEntity::ET_addChild, rightBox->getEntityId());
    }

    CheckUIBoxSizeAndCenter(leftBox, Vec2i(100), Vec2i(50));
    CheckUIBoxSizeAndCenter(rightBox, Vec2i(100), Vec2i(160, 50));
}