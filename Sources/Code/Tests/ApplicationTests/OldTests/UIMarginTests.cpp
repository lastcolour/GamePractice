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

TEST_F(UIMarginTests, CheckRightMargin) {
    auto leftBox = createObjectAndLogic<UIBaseBox>();
    {
        UIStyle style;
        style.size = Vec2(100.f);
        style.sizeInv = SizeInvariant::Pixel;
        style.margin.right = 20.f;
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
    CheckUIBoxSizeAndCenter(rightBox, Vec2i(100), Vec2i(170, 50));
}

TEST_F(UIMarginTests, CheckBotMargin) {
    auto topBox = createObjectAndLogic<UIBaseBox>();
    {
        UIStyle style;
        style.size = Vec2(100.f);
        style.sizeInv = SizeInvariant::Pixel;
        style.margin.bot = 10.f;
        topBox->ET_setStyle(style);
        ASSERT_TRUE(topBox->init());
    }
    auto botBox = createObjectAndLogic<UIBaseBox>();
    {
        UIStyle style;
        style.size = Vec2(100.f);
        style.sizeInv = SizeInvariant::Pixel;
        botBox->ET_setStyle(style);
        ASSERT_TRUE(botBox->init());
    }
    {
        auto uiList = createObjectAndLogic<UIList>();
        uiList->ET_setType(UIListType::Vertical);

        UIStyle style;
        style.xAlignType = XAlignType::Left;
        style.yAlignType = YAlignType::Bot;
        uiList->ET_setStyle(style);

        ASSERT_TRUE(uiList->init());
        ET_SendEvent(uiList->getEntityId(), &ETEntity::ET_addChild, topBox->getEntityId());
        ET_SendEvent(uiList->getEntityId(), &ETEntity::ET_addChild, botBox->getEntityId());
    }

    CheckUIBoxSizeAndCenter(topBox, Vec2i(100), Vec2i(50, 160));
    CheckUIBoxSizeAndCenter(botBox, Vec2i(100), Vec2i(50));
}

TEST_F(UIMarginTests, CheckTopMargin) {
    auto topBox = createObjectAndLogic<UIBaseBox>();
    {
        UIStyle style;
        style.size = Vec2(100.f);
        style.sizeInv = SizeInvariant::Pixel;
        style.margin.bot = 10.f;
        topBox->ET_setStyle(style);
        ASSERT_TRUE(topBox->init());
    }
    auto botBox = createObjectAndLogic<UIBaseBox>();
    {
        UIStyle style;
        style.size = Vec2(100.f);
        style.sizeInv = SizeInvariant::Pixel;
        style.margin.top = 20.f;
        botBox->ET_setStyle(style);
        ASSERT_TRUE(botBox->init());
    }
    {
        auto uiList = createObjectAndLogic<UIList>();
        uiList->ET_setType(UIListType::Vertical);

        UIStyle style;
        style.xAlignType = XAlignType::Left;
        style.yAlignType = YAlignType::Bot;
        uiList->ET_setStyle(style);

        ASSERT_TRUE(uiList->init());
        ET_SendEvent(uiList->getEntityId(), &ETEntity::ET_addChild, topBox->getEntityId());
        ET_SendEvent(uiList->getEntityId(), &ETEntity::ET_addChild, botBox->getEntityId());
    }

    CheckUIBoxSizeAndCenter(topBox, Vec2i(100), Vec2i(50, 170));
    CheckUIBoxSizeAndCenter(botBox, Vec2i(100), Vec2i(50));
}