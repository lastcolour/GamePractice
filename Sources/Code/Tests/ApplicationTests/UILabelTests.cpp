#include "UILabelTests.hpp"

#include <Entity/Entity.hpp>
#include <UI/Logics/UILabel.hpp>
#include <UI/Logics/UIList.hpp>

namespace {

const char* TEST_TEXT_RENDERER = "Render/RenderTextSimple.json";
const float TEST_FONT_SIZE = 1.f;

} // namespace

UILabel* UILabelTests::createUILabel() {
    auto object = createVoidObject();
    std::unique_ptr<UILabel> uiLabelPtr(new UILabel);
    UILabel* uiLabel = uiLabelPtr.get();
    object->addCustomLogic(std::move(uiLabelPtr));
    tempObject.push_back(std::move(object));
    return uiLabel;
}

UIList* UILabelTests::createUIList() {
    auto object = createVoidObject();
    std::unique_ptr<UIList> uiListPtr(new UIList);
    UIList* uiLabel = uiListPtr.get();
    object->addCustomLogic(std::move(uiListPtr));
    tempObject.push_back(std::move(object));
    return uiLabel;
}

TEST_F(UILabelTests, CheckLabelConnection) {
    auto uiLabel = createUILabel();
    UIStyle style;
    style.fontSize = TEST_FONT_SIZE;
    style.renderer = TEST_TEXT_RENDERER;
    uiLabel->ET_setStyle(style);
    ASSERT_TRUE(uiLabel->init());

    ASSERT_TRUE(ET_IsExistNode<ETUIBox>(uiLabel->getEntityId()));
    ASSERT_TRUE(ET_IsExistNode<ETUILabel>(uiLabel->getEntityId()));
}

TEST_F(UILabelTests, CheckLabelBox) {
    auto uiLabel = createUILabel();
    UIStyle style;
    style.fontSize = TEST_FONT_SIZE;
    style.renderer = TEST_TEXT_RENDERER;
    uiLabel->ET_setStyle(style);
    ASSERT_TRUE(uiLabel->init());

    auto emptyBox = uiLabel->ET_getAabb2di();
    auto emptySize = emptyBox.getSize();
    EXPECT_EQ(emptySize.x, 0);
    EXPECT_EQ(emptySize.y, 0);

    uiLabel->ET_setText("A");

    auto aBox = uiLabel->ET_getAabb2di();
    auto aBoxSize = aBox.getSize();
    EXPECT_GT(aBoxSize.x, 0);
    EXPECT_GT(aBoxSize.y, 0);

    uiLabel->ET_setText("Aa");

    auto aaBox = uiLabel->ET_getAabb2di();
    auto aaBoxSize = aaBox.getSize();
    EXPECT_GT(aaBoxSize.x, aBoxSize.x);
    EXPECT_EQ(aaBoxSize.y, aBoxSize.y);
}

TEST_F(UILabelTests, CheckLabelLocation) {
    auto uiLabel = createUILabel();
    uiLabel->ET_setText("A");
    UIStyle style;
    style.fontSize = TEST_FONT_SIZE;
    style.renderer = TEST_TEXT_RENDERER;
    uiLabel->ET_setStyle(style);
    ASSERT_TRUE(uiLabel->init());

    Transform tm;
    ET_SendEventReturn(tm, uiLabel->getEntityId(), &ETEntity::ET_getTransform);

    Vec2i renderPort;
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    Vec3 midPt = Vec3(renderPort.x / 2.f, renderPort.y / 2.f, 0.f);
    EXPECT_EQ(tm.pt, midPt); 
}

TEST_F(UILabelTests, CheckLabelCapitalSize) {
    auto uiLabel = createUILabel();
    UIStyle style;
    style.fontSize = TEST_FONT_SIZE;
    style.renderer = TEST_TEXT_RENDERER;
    uiLabel->ET_setStyle(style);
    uiLabel->ET_setText("A");
    ASSERT_TRUE(uiLabel->init());

    auto capitalABox = uiLabel->ET_getAabb2di();
    auto capitalABoxSize = capitalABox.getSize();

    uiLabel->ET_setText("a");

    auto smallABox = uiLabel->ET_getAabb2di();
    auto smallABoxSize = smallABox.getSize();

    EXPECT_LE(smallABoxSize.x, capitalABoxSize.x);
    EXPECT_EQ(smallABoxSize.y, capitalABoxSize.y);
}

TEST_F(UILabelTests, CheckLineBreak) {
    auto uiLabel = createUILabel();
    UIStyle style;
    style.fontSize = TEST_FONT_SIZE;
    style.renderer = TEST_TEXT_RENDERER;
    uiLabel->ET_setStyle(style);
    uiLabel->ET_setText("A");
    ASSERT_TRUE(uiLabel->init());

    auto aBox = uiLabel->ET_getAabb2di();
    auto aBoxSize = aBox.getSize();

    uiLabel->ET_setText("A\nA");

    auto aaBox = uiLabel->ET_getAabb2di();
    auto aaBoxSize = aaBox.getSize();

    EXPECT_EQ(aBox.getCenter(), aaBox.getCenter());
    EXPECT_EQ(aaBoxSize.x, aBoxSize.x);
    EXPECT_GT(aaBoxSize.y, 2 * aBoxSize.y);
}

TEST_F(UILabelTests, CheckLabelResize) {
    auto uiLabel = createUILabel();
    uiLabel->ET_setText("A");
    UIStyle style;
    style.fontSize = TEST_FONT_SIZE;
    style.renderer = TEST_TEXT_RENDERER;
    uiLabel->ET_setStyle(style);
    ASSERT_TRUE(uiLabel->init());

    auto smallBox = uiLabel->ET_getAabb2di();
    auto smallBoxSize = smallBox.getSize();

    style.fontSize = style.fontSize * 2.f;
    uiLabel->ET_setStyle(style);

    auto bigBox = uiLabel->ET_getAabb2di();
    auto bigBoxSize = bigBox.getSize();

    EXPECT_GT(bigBoxSize.x, smallBoxSize.x);
    EXPECT_GT(bigBoxSize.y, smallBoxSize.y);

    EXPECT_EQ(bigBox.getCenter(), smallBox.getCenter());
}

TEST_F(UILabelTests, CheckLabelResizeInsideList) {
    auto uiList = createUIList();
    {
        UIStyle style;
        style.xAlignType = XAlignType::Center;
        style.yAlignType = YAlignType::Center;
        style.size = Vec2(200.f);
        style.sizeInv = SizeInvariant::Pixel;
        uiList->ET_setStyle(style);
        uiList->ET_setType(UIListType::Horizontal);
        ASSERT_TRUE(uiList->init());
    }
    auto uiLabel = createUILabel();
    {
        UIStyle style;
        style.fontSize = 1.f;
        uiLabel->ET_setText("Ap|");
        uiLabel->ET_setStyle(style);
        ASSERT_TRUE(uiLabel->init());
    }

    ET_SendEvent(uiList->getEntityId(), &ETEntity::ET_addChild, uiLabel->getEntityId());

    auto box = uiLabel->ET_getAabb2di();
    auto boxSize = box.getSize();
    auto boxCenter = box.getCenter();

    EXPECT_GT(boxSize.x, 100);
    EXPECT_EQ(boxSize.y, 200);

    {
        UIStyle style;
        style.fontSize = 0.5f;
        uiLabel->ET_setStyle(style);
    }

    box = uiLabel->ET_getAabb2di();
    boxSize = box.getSize();
    boxCenter = box.getCenter();

    EXPECT_GT(boxSize.x, 50);
    EXPECT_EQ(boxSize.y, 100);
}

TEST_F(UILabelTests, CheckListOfLabels) {
    auto uiList = createUIList();
    {
        UIStyle style;
        style.xAlignType = XAlignType::Center;
        style.yAlignType = YAlignType::Center;
        style.size = Vec2(0.5f);
        style.sizeInv = SizeInvariant::Relative;
        uiList->ET_setStyle(style);
        uiList->ET_setType(UIListType::Horizontal);
        ASSERT_TRUE(uiList->init());
    }
    auto uiFirstLabel = createUILabel();
    auto uiFirstInnnerList = createUIList();
    {
        {
            UIStyle style;
            style.xAlignType = XAlignType::Center;
            style.yAlignType = YAlignType::Center;
            style.size = Vec2(0.5f);
            style.sizeInv = SizeInvariant::Relative;
            uiFirstInnnerList->ET_setStyle(style);
            uiFirstInnnerList->ET_setType(UIListType::Horizontal);
            ASSERT_TRUE(uiFirstInnnerList->init());
        }
        {
            UIStyle style;
            style.fontSize = 0.5f;
            uiFirstLabel->ET_setText("A");
            uiFirstLabel->ET_setStyle(style);
            ASSERT_TRUE(uiFirstLabel->init());
        }
        ET_SendEvent(uiFirstInnnerList->getEntityId(), &ETEntity::ET_addChild, uiFirstLabel->getEntityId());
    }
    auto uiSecondLabel = createUILabel();
    auto uiSecondInnerList = createUIList();
    {
        {
            UIStyle style;
            style.xAlignType = XAlignType::Center;
            style.yAlignType = YAlignType::Center;
            style.size = Vec2(0.5f);
            style.sizeInv = SizeInvariant::Relative;
            uiSecondInnerList->ET_setStyle(style);
            uiSecondInnerList->ET_setType(UIListType::Horizontal);
            ASSERT_TRUE(uiSecondInnerList->init());
        }
        {
            UIStyle style;
            style.fontSize = 0.5f;
            uiSecondLabel->ET_setText("A");
            uiSecondLabel->ET_setStyle(style);
            ASSERT_TRUE(uiSecondLabel->init());
        }
        ET_SendEvent(uiSecondInnerList->getEntityId(), &ETEntity::ET_addChild, uiSecondLabel->getEntityId());
    }

    ET_SendEvent(uiList->getEntityId(), &ETEntity::ET_addChild, uiFirstInnnerList->getEntityId());
    ET_SendEvent(uiList->getEntityId(), &ETEntity::ET_addChild, uiSecondInnerList->getEntityId());

    auto fisrtBox = uiFirstLabel->ET_getAabb2di();
    auto firstBoxSize = fisrtBox.getSize();

    auto secondBox = uiSecondLabel->ET_getAabb2di();
    auto secondBoxSize = secondBox.getSize();

    EXPECT_EQ(firstBoxSize.x, secondBoxSize.x);
    EXPECT_EQ(firstBoxSize.y, secondBoxSize.y);
}