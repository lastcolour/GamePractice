#include "UILabelTests.hpp"

#include <Game/GameObject.hpp>
#include <UI/Logics/UILabel.hpp>

UILabel* UILabelTests::createUILabel() {
    auto object = createVoidObject();
    std::unique_ptr<UILabel> uiButtonPtr(new UILabel);
    UILabel* uiLabel = uiButtonPtr.get();
    object->addLogic(std::move(uiButtonPtr));
    tempObject.push_back(std::move(object));
    return uiLabel;
}

TEST_F(UILabelTests, CheckLabelConnection) {
    auto uiLabel = createUILabel();
    ASSERT_TRUE(uiLabel->init());

    ASSERT_TRUE(ET_IsExistNode<ETUIBox>(uiLabel->getEntityId()));
    ASSERT_TRUE(ET_IsExistNode<ETUILabel>(uiLabel->getEntityId()));
}

TEST_F(UILabelTests, CheckLabelBox) {
    auto uiLabel = createUILabel();
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
    EXPECT_GT(aaBoxSize.y, aBoxSize.y);
}

TEST_F(UILabelTests, CheckLabelLocation) {
    auto uiLabel = createUILabel();
    uiLabel->ET_setText("A");
    UIStyle style;
    style.xAlignType = XAlignType::Center;
    style.yAlignType = YAlignType::Center;
    uiLabel->ET_setStyle(style);
    ASSERT_TRUE(uiLabel->init());

    Transform tm;
    ET_SendEventReturn(tm, uiLabel->getEntityId(), &ETGameObject::ET_getTransform);

    Vec2i renderPort;
    ET_SendEventReturn(renderPort, &ETRender::ET_getRenderPort);

    Vec3 midPt = Vec3(renderPort.x / 2.f, renderPort.y / 2.f, 0.f);
    EXPECT_EQ(tm.pt, midPt); 
}

TEST_F(UILabelTests, CheckLabelResize) {

}