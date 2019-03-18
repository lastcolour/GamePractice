#include "UITests.hpp"
#include "UI/Logics/UIBox.hpp"
#include "UI/Logics/UIList.hpp"
#include "Core/JSONNode.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Game/GameETInterfaces.hpp"
#include "Game/GameObject.hpp"

class TestUIBox : public UIBox {
public:
    virtual ~TestUIBox() = default;

    void setStyle(const UIStyle& uiStyle) {
        UIBox::setStyle(uiStyle);
    }

protected:

    bool serialize(const JSONNode& node) override { return true; }
};

class TestUIList : public UIList {
public:
    virtual ~TestUIList() = default;

    void setStyle(const UIStyle& uiStyle) {
        UIList::setStyle(uiStyle);
    }
    void setType(ListType type) {
        listType = type;
    }

protected:

    bool serialize(const JSONNode& node) override { return true; }
};

void UITests::SetUp() {
}

void UITests::TearDown() {
    tempObject.clear();
}

TestUIList* UITests::createUIList() {
    auto object = createVoidObject();
    std::unique_ptr<TestUIList> uiListPtr(new TestUIList);
    TestUIList* uiList = uiListPtr.get();
    object->addLogic(std::move(uiListPtr));
    tempObject.push_back(std::move(object));
    return uiList;
}

TestUIBox* UITests::createUIBox() {
    auto object = createVoidObject();
    std::unique_ptr<TestUIBox> uiBoxPtr(new TestUIBox);
    TestUIBox* uiBox = uiBoxPtr.get();
    object->addLogic(std::move(uiBoxPtr));
    tempObject.push_back(std::move(object));
    return uiBox;
}

TEST_F(UITests, CheckXAlign) {
    TestUIBox* uiBox = createUIBox();
    UIStyle style;
    style.size = Vec2(0.5f);
    style.yAlignType = YAlignType::Center;
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRender::ET_getRenderPort);

    style.xAlignType = XAlignType::Center;
    uiBox->setStyle(style);
    ASSERT_TRUE(uiBox->init());
    AABB2Di aabb = uiBox->ET_getAabb2di();
    ASSERT_EQ(aabb.getCenter(), renderPort / 2);

    style.xAlignType = XAlignType::Left;
    uiBox->setStyle(style);
    ASSERT_TRUE(uiBox->init());
    aabb = uiBox->ET_getAabb2di();
    ASSERT_EQ(aabb.getCenter(), Vec2i(renderPort.x / 4, renderPort.y / 2));

    style.xAlignType = XAlignType::Right;
    uiBox->setStyle(style);
    ASSERT_TRUE(uiBox->init());
    aabb = uiBox->ET_getAabb2di();
    ASSERT_EQ(aabb.getCenter(), Vec2i(3 * renderPort.x / 4, renderPort.y / 2));
}

TEST_F(UITests, CheckYAlign) {
    TestUIBox* uiBox = createUIBox();
    UIStyle style;
    style.size = Vec2(0.5f);
    style.xAlignType = XAlignType::Center;
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRender::ET_getRenderPort);

    style.yAlignType = YAlignType::Top;
    uiBox->setStyle(style);
    ASSERT_TRUE(uiBox->init());
    AABB2Di aabb = uiBox->ET_getAabb2di();
    ASSERT_EQ(aabb.getCenter(), Vec2i(renderPort.x / 2, 3 * renderPort.y / 4));

    style.yAlignType = YAlignType::Bot;
    uiBox->setStyle(style);
    ASSERT_TRUE(uiBox->init());
    aabb = uiBox->ET_getAabb2di();
    ASSERT_EQ(aabb.getCenter(), Vec2i(renderPort.x / 2, renderPort.y / 4));
}

TEST_F(UITests, CheckUIBoxSizeInvariants) {
    TestUIBox* uiBox = createUIBox();
    UIStyle style;
    style.size = Vec2(0.5f, 0.5f);
    AABB2Di aabb(0);
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRender::ET_getRenderPort);
    int minSide = std::min(renderPort.x, renderPort.y);

    style.sizeInv = SizeInvariant::Absolute;
    uiBox->setStyle(style);
    ASSERT_TRUE(uiBox->init());
    aabb = uiBox->ET_getAabb2di();
    ASSERT_EQ(aabb.getSize(), Vec2i(renderPort.x / 2, renderPort.y / 2));
    ASSERT_EQ(aabb.getCenter(), renderPort / 2);

    style.sizeInv = SizeInvariant::AbsoluteBiggestSquare;
    uiBox->setStyle(style);
    ASSERT_TRUE(uiBox->init());
    aabb = uiBox->ET_getAabb2di();
    ASSERT_EQ(aabb.getSize(), Vec2i(minSide / 2));
    ASSERT_EQ(aabb.getCenter(), renderPort / 2);

    style.sizeInv = SizeInvariant::Pixel;
    style.size = Vec2(100.f, 300.f);
    uiBox->setStyle(style);
    ASSERT_TRUE(uiBox->init());
    aabb = uiBox->ET_getAabb2di();
    ASSERT_EQ(aabb.getSize(), Vec2i(100, 300));
    ASSERT_EQ(aabb.getCenter(), renderPort / 2);

    TestUIBox* parentUIBox = createUIBox();
    style.sizeInv = SizeInvariant::Relative;
    style.size = Vec2(0.5f, 0.5f);
    parentUIBox->setStyle(style);
    ASSERT_TRUE(parentUIBox->init());
    ET_SendEvent(parentUIBox->getEntityId(), &ETGameObject::ET_addChild, uiBox->getEntityId());

    style.sizeInv = SizeInvariant::Relative;
    style.size = Vec2(0.5f, 0.5f);
    uiBox->setStyle(style);
    ASSERT_TRUE(uiBox->init());
    aabb = uiBox->ET_getAabb2di();
    ASSERT_EQ(aabb.getSize(), Vec2i(renderPort.x / 4, renderPort.y / 4));
    ASSERT_EQ(aabb.getCenter(), renderPort / 2);

    style.sizeInv = SizeInvariant::RelativeBiggestSquare;
    uiBox->setStyle(style);
    ASSERT_TRUE(uiBox->init());
    aabb = uiBox->ET_getAabb2di();
    ASSERT_EQ(aabb.getSize(), Vec2i(minSide / 4));
    ASSERT_EQ(aabb.getCenter(), renderPort / 2);
}

TEST_F(UITests, CheckUIBoxInsideUIBox) {
    TestUIBox* uiBox1 = createUIBox();
    TestUIBox* uiBox2 = createUIBox();

    ET_SendEvent(uiBox1->getEntityId(), &ETGameObject::ET_addChild, uiBox2->getEntityId());

    UIStyle style;
    style.size = Vec2(0.5f, 0.5f);

    uiBox1->setStyle(style);
    ASSERT_TRUE(uiBox1->init());

    uiBox2->setStyle(style);
    ASSERT_TRUE(uiBox2->init());

    auto& parentBox = uiBox1->ET_getAabb2di();
    auto parentSize = parentBox.getSize();
    auto& aabb = uiBox2->ET_getAabb2di();

    ASSERT_EQ(aabb.getSize(), Vec2i(parentSize / 2));
    ASSERT_EQ(aabb.getCenter(), parentBox.getCenter());
}

TEST_F(UITests, CheckVerticalUIList) {
    TestUIList* uiList = createUIList();
    uiList->setType(ListType::Vertical);
    ASSERT_TRUE(uiList->init());

    UIStyle boxStyle;
    boxStyle.size = Vec2(0.25f, 0.25f);

    TestUIBox* uiBox1 = createUIBox();
    uiBox1->setStyle(boxStyle);
    ASSERT_TRUE(uiBox1->init());

    TestUIBox* uiBox2 = createUIBox();
    uiBox2->setStyle(boxStyle);
    ASSERT_TRUE(uiBox2->init());

    uiList->ET_addElement(uiBox1->getEntityId());
    uiList->ET_addElement(uiBox2->getEntityId());

    const auto& aabbList = uiList->ET_getAabb2di();

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRender::ET_getRenderPort);

    Vec2i expCenter = renderPort / 2;
    Vec2i resCenter = aabbList.getCenter();
    ASSERT_EQ(resCenter, expCenter);
    Vec2i expSize = Vec2i(renderPort.x / 4, renderPort.y / 2);
    Vec2i resSize = aabbList.getSize();
    ASSERT_EQ(resSize, expSize);

    const auto& aabbBox1 = uiBox1->ET_getAabb2di();
    expCenter = Vec2i(renderPort.x / 2, 5 * renderPort.y / 8);
    resCenter = aabbBox1.getCenter();
    ASSERT_EQ(resCenter, expCenter);
    expSize = renderPort / 4;
    resSize = aabbBox1.getSize();
    ASSERT_EQ(resSize, expSize);

    const auto& aabbBox2 = uiBox2->ET_getAabb2di();
    expCenter = Vec2i(renderPort.x / 2, 3 * renderPort.y / 8);
    resCenter = aabbBox2.getCenter();
    ASSERT_EQ(resCenter, expCenter);
    expSize = renderPort / 4;
    resSize = aabbBox2.getSize();
    ASSERT_EQ(resSize, expSize);
}

TEST_F(UITests, CheckHorizontalUIList) {
    TestUIList* uiList = createUIList();
    uiList->setType(ListType::Horizontal);
    ASSERT_TRUE(uiList->init());

    UIStyle boxStyle;
    boxStyle.size = Vec2(0.25f, 0.25f);

    TestUIBox* uiBox1 = createUIBox();
    uiBox1->setStyle(boxStyle);
    ASSERT_TRUE(uiBox1->init());

    TestUIBox* uiBox2 = createUIBox();
    uiBox2->setStyle(boxStyle);
    ASSERT_TRUE(uiBox2->init());

    uiList->ET_addElement(uiBox1->getEntityId());
    uiList->ET_addElement(uiBox2->getEntityId());

    const auto& aabbList = uiList->ET_getAabb2di();

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRender::ET_getRenderPort);

    Vec2i expCenter = renderPort / 2;
    Vec2i resCenter = aabbList.getCenter();
    ASSERT_EQ(resCenter, expCenter);
    Vec2i expSize = Vec2i(renderPort.x / 2, renderPort.y / 4);
    Vec2i resSize = aabbList.getSize();
    ASSERT_EQ(resSize, expSize);

    const auto& aabbBox1 = uiBox1->ET_getAabb2di();
    expCenter = Vec2i(3 * renderPort.x / 8, renderPort.y / 2);
    resCenter = aabbBox1.getCenter();
    ASSERT_EQ(resCenter, expCenter);
    expSize = renderPort / 4;
    resSize = aabbBox1.getSize();
    ASSERT_EQ(resSize, expSize);

    const auto& aabbBox2 = uiBox2->ET_getAabb2di();
    expCenter = Vec2i(5 * renderPort.x / 8, renderPort.y / 2);
    resCenter = aabbBox2.getCenter();
    ASSERT_EQ(resCenter, expCenter);
    expSize = renderPort / 4;
    resSize = aabbBox2.getSize();
    ASSERT_EQ(resSize, expSize);
}