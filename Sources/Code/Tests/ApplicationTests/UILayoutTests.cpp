#include "UILayoutTests.hpp"
#include "Logics/UILayout.hpp"
#include "Logics/UIBox.hpp"
#include "Config/UIConfig.hpp"
#include "Logics/UILayoutBox.hpp"
#include "Core/ETApplication.hpp"

namespace {

void CheckUIBox(EntityId entityId, Vec2i expCenter, Vec2i expSize) {
    AABB2Di box;
    ET_SendEventReturn(box, entityId, &ETUIElement::ET_getBox);

    auto center = box.getCenter();

    EXPECT_EQ(center.x, expCenter.x);
    EXPECT_EQ(center.y, expCenter.y);

    auto size = box.getSize();

    EXPECT_EQ(size.x, expSize.x);
    EXPECT_EQ(size.y, expSize.y);
}

class TestLayoutEventListener : public ETNode<ETUILayoutEvents> {
public:

    TestLayoutEventListener() :
        layoutChanged(false) {}
    virtual ~TestLayoutEventListener() {}

    void listerUpdated(EntityId layoutEntId) {
        ETNode<ETUILayoutEvents>::connect(layoutEntId);
    }

    // ETUILayoutEvents
    void ET_onLayoutChanged(const AABB2Di& newCombinedBox) override {
        layoutChanged = true;
    }

public:

    bool layoutChanged;
};

} // namespace

Entity* UILayoutTests::createUIBox(float width, float height) {
    auto entity = createVoidObject();
    auto rootBoxPtr = entity->addCustomLogic<UIBox>();
    HACK_ASSERT_TRUE(rootBoxPtr);

    UIBoxStyle boxStyle;
    boxStyle.height = width;
    boxStyle.heightInv = UIBoxSizeInvariant::Relative;
    boxStyle.width = height;
    boxStyle.widthInv = UIBoxSizeInvariant::Relative;
    rootBoxPtr->ET_setStyle(boxStyle);

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    Transform tm;
    tm.pt = Vec3(viewPort.x / 2.f, viewPort.y / 2.f, 0.f);
    ET_SendEvent(entity->getEntityId(), &ETEntity::ET_setTransform, tm);

    return entity;
}

Entity* UILayoutTests::createUILayoutBox() {
    auto entity = createVoidObject();
    HACK_ASSERT_TRUE(entity->addCustomLogic<UILayoutBox>());

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    Transform tm;
    tm.pt = Vec3(viewPort.x / 2.f, viewPort.y / 2.f, 0.f);
    ET_SendEvent(entity->getEntityId(), &ETEntity::ET_setTransform, tm);

    return entity;
}

UILayout* UILayoutTests::addUILayout(Entity* entity, UILayoutType type, UIXAlign xAlign, UIYAlign yAlign) {
    auto layoutPtr = entity->addCustomLogic<UILayout>();
    HACK_ASSERT_TRUE(layoutPtr);

    UILayoutStyle layoutStyle;
    layoutStyle.type = type;
    layoutStyle.xAlign = xAlign;
    layoutStyle.yAlign = yAlign;
    layoutPtr->ET_setStyle(layoutStyle);
    return layoutPtr;
}

TEST_F(UILayoutTests, CheckVerticalLayout) {
    auto rootEntity = createUIBox(1.f, 1.f);
    auto rootLayout = addUILayout(rootEntity, UILayoutType::Vertical, UIXAlign::Center, UIYAlign::Center);

    auto firstChild = createUIBox(0.25f, 0.25f);
    auto secondChild = createUIBox(0.25f, 0.25f);

    rootLayout->ET_addItem(firstChild->getEntityId());
    rootLayout->ET_addItem(secondChild->getEntityId());

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    {
        AABB2Di box;
        ET_SendEventReturn(box, firstChild->getEntityId(), &ETUIElement::ET_getBox);
        Vec2i expCenter = Vec2i(viewPort.x / 2, 5 * viewPort.y / 8);
        Vec2i resCenter = box.getCenter();
        EXPECT_EQ(resCenter, expCenter);
        Vec2i expSize = viewPort / 4;
        Vec2i resSize = box.getSize();
        EXPECT_EQ(resSize, expSize);
    }
    {
        AABB2Di box;
        ET_SendEventReturn(box, secondChild->getEntityId(), &ETUIElement::ET_getBox);
        Vec2i expCenter = Vec2i(viewPort.x / 2, 3 * viewPort.y / 8);
        Vec2i resCenter = box.getCenter();
        EXPECT_EQ(resCenter, expCenter);
        Vec2i expSize = viewPort / 4;
        Vec2i resSize = box.getSize();
        EXPECT_EQ(resSize, expSize);
    }
}

TEST_F(UILayoutTests, CheckHorizontalLayout) {
    auto rootEntity = createUIBox(1.f, 1.f);
    auto rootLayout = addUILayout(rootEntity, UILayoutType::Horizontal, UIXAlign::Center, UIYAlign::Center);

    auto firstChild = createUIBox(0.25f, 0.25f);
    auto secondChild = createUIBox(0.25f, 0.25f);

    rootLayout->ET_addItem(firstChild->getEntityId());
    rootLayout->ET_addItem(secondChild->getEntityId());

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    {
        AABB2Di box;
        ET_SendEventReturn(box, firstChild->getEntityId(), &ETUIElement::ET_getBox);
        Vec2i expCenter = Vec2i(3 * viewPort.x / 8, viewPort.y / 2);
        Vec2i resCenter = box.getCenter();
        EXPECT_EQ(resCenter, expCenter);
        Vec2i expSize = viewPort / 4;
        Vec2i resSize = box.getSize();
        EXPECT_EQ(resSize, expSize);
    }
    {
        AABB2Di box;
        ET_SendEventReturn(box, secondChild->getEntityId(), &ETUIElement::ET_getBox);
        Vec2i expCenter = Vec2i(5 * viewPort.x / 8, viewPort.y / 2);
        Vec2i resCenter = box.getCenter();
        EXPECT_EQ(resCenter, expCenter);
        Vec2i expSize = viewPort / 4;
        Vec2i resSize = box.getSize();
        EXPECT_EQ(resSize, expSize);
    }
}

TEST_F(UILayoutTests, CheckAlign) {
    auto rootEntity = createUIBox(1.f, 1.f);
    auto rootLayout = addUILayout(rootEntity, UILayoutType::Horizontal, UIXAlign::Center, UIYAlign::Center);

    auto child = createUIBox(0.5f, 0.5f);

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    rootLayout->ET_addItem(child->getEntityId());

    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Center;
        style.yAlign = UIYAlign::Bot;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIElement::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(viewPort.x / 2, viewPort.y / 4);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Center;
        style.yAlign = UIYAlign::Center;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIElement::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(viewPort.x / 2, viewPort.y / 2);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Center;
        style.yAlign = UIYAlign::Top;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIElement::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(viewPort.x / 2, 3 * viewPort.y / 4);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Left;
        style.yAlign = UIYAlign::Bot;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIElement::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(viewPort.x / 4, viewPort.y / 4);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Left;
        style.yAlign = UIYAlign::Center;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIElement::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(viewPort.x / 4, viewPort.y / 2);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Left;
        style.yAlign = UIYAlign::Top;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIElement::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(viewPort.x / 4, 3 * viewPort.y / 4);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Right;
        style.yAlign = UIYAlign::Bot;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIElement::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(3 * viewPort.x / 4, viewPort.y / 4);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Right;
        style.yAlign = UIYAlign::Center;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIElement::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(3 * viewPort.x / 4, viewPort.y / 2);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Right;
        style.yAlign = UIYAlign::Top;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIElement::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(3 * viewPort.x / 4, 3 * viewPort.y / 4);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
}

TEST_F(UILayoutTests, CheckMargin) {
    auto rootEntity = createUIBox(1.f, 1.f);
    auto rootLayout = addUILayout(rootEntity, UILayoutType::Horizontal, UIXAlign::Center, UIYAlign::Center);

    auto first = createUIBox(0.25f, 0.25f);
    auto second = createUIBox(0.25, 0.25f);

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    const auto boxSize = viewPort / 4;
    const auto center = viewPort / 2;
    const float margin = 10.f;
    auto halfShift = ET_getShared<UIConfig>()->getSizeOnGrind(10.f) / 2;

    rootLayout->ET_addItem(first->getEntityId());
    rootLayout->ET_addItem(second->getEntityId());

    UIBoxStyle boxStyle;
    ET_SendEventReturn(boxStyle, first->getEntityId(), &ETUIBox::ET_getStyle);

    {
        boxStyle.margin.right = margin;
        ET_SendEvent(first->getEntityId(), &ETUIBox::ET_setStyle, boxStyle);

        CheckUIBox(first->getEntityId(), Vec2i(center.x - halfShift - boxSize.x / 2, center.y), boxSize);
        CheckUIBox(second->getEntityId(), Vec2i(center.x + halfShift + boxSize.x / 2, center.y), boxSize);
    }

    {
        boxStyle.margin.right = 0.f;
        ET_SendEvent(first->getEntityId(), &ETUIBox::ET_setStyle, boxStyle);

        boxStyle.margin.left = margin;
        ET_SendEvent(second->getEntityId(), &ETUIBox::ET_setStyle, boxStyle);

        CheckUIBox(first->getEntityId(), Vec2i(center.x - halfShift - boxSize.x / 2, center.y), boxSize);
        CheckUIBox(second->getEntityId(), Vec2i(center.x + halfShift + boxSize.x / 2, center.y), boxSize);
    }

    {
        UILayoutStyle layoutStyle;
        ET_SendEventReturn(layoutStyle, rootEntity->getEntityId(), &ETUILayout::ET_getStyle);
        layoutStyle.type = UILayoutType::Vertical;
        ET_SendEvent(rootEntity->getEntityId(), &ETUILayout::ET_setStyle, layoutStyle);
    }

    {
        boxStyle.margin.bot = margin;
        ET_SendEvent(first->getEntityId(), &ETUIBox::ET_setStyle, boxStyle);

        boxStyle.margin.left = 0.f;
        ET_SendEvent(second->getEntityId(), &ETUIBox::ET_setStyle, boxStyle);

        CheckUIBox(first->getEntityId(), Vec2i(center.x, center.y + halfShift + boxSize.y / 2), boxSize);
        CheckUIBox(second->getEntityId(), Vec2i(center.x, center.y - halfShift - boxSize.y / 2), boxSize);
    }

    {
        boxStyle.margin.bot = 0.f;
        ET_SendEvent(first->getEntityId(), &ETUIBox::ET_setStyle, boxStyle);

        boxStyle.margin.top = margin;
        ET_SendEvent(second->getEntityId(), &ETUIBox::ET_setStyle, boxStyle);

        CheckUIBox(first->getEntityId(), Vec2i(center.x, center.y + halfShift + boxSize.y / 2), boxSize);
        CheckUIBox(second->getEntityId(), Vec2i(center.x, center.y - halfShift - boxSize.y / 2), boxSize);
    }
}

TEST_F(UILayoutTests, CheckUILayoutOutMargin) {
    auto rootEntity = createUIBox(1.f, 1.f);
    auto childEntity = createUIBox(0.5f, 0.5f);

    auto rootLayout = addUILayout(rootEntity, UILayoutType::Horizontal, UIXAlign::Center, UIYAlign::Center);
    rootLayout->ET_addItem(childEntity->getEntityId());

    {
        UIBoxStyle boxStyle;
        ET_SendEventReturn(boxStyle, childEntity->getEntityId(), &ETUIBox::ET_getStyle);

        boxStyle.margin.bot = 2.f;
        boxStyle.margin.top = 2.f;
        boxStyle.margin.left = 2.f;
        boxStyle.margin.right = 2.f;
        ET_SendEvent(childEntity->getEntityId(), &ETUIBox::ET_setStyle, boxStyle);
    }

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    auto shift = ET_getShared<UIConfig>()->getSizeOnGrind(2.f);
    const auto center = viewPort / 2;
    const auto boxSize = center;

    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Left;
        style.yAlign = UIYAlign::Center;
        rootLayout->ET_setStyle(style);

        CheckUIBox(childEntity->getEntityId(), Vec2i(shift + boxSize.x / 2, center.y), boxSize);
    }

    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Right;
        style.yAlign = UIYAlign::Center;
        rootLayout->ET_setStyle(style);

        CheckUIBox(childEntity->getEntityId(), Vec2i(viewPort.x - boxSize.x / 2 - shift, center.y), boxSize);
    }

    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Center;
        style.yAlign = UIYAlign::Top;
        rootLayout->ET_setStyle(style);

        CheckUIBox(childEntity->getEntityId(), Vec2i(center.x, viewPort.y - boxSize.y / 2 - shift), boxSize);
    }

    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Center;
        style.yAlign = UIYAlign::Bot;
        rootLayout->ET_setStyle(style);

        CheckUIBox(childEntity->getEntityId(), Vec2i(center.x, shift + boxSize.y / 2), boxSize);
    }
}

TEST_F(UILayoutTests, CheckLayoutOnLayoutBox) {
    auto layoutBox = createUILayoutBox();
    auto rootLayout = addUILayout(layoutBox, UILayoutType::Vertical, UIXAlign::Center, UIYAlign::Center);

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    Vec2i center = viewPort / 2;

    {
        CheckUIBox(layoutBox->getEntityId(), center, Vec2i(0));
    }

    auto firstBox = createUIBox(0.5f, 0.5f);
    rootLayout->ET_addItem(firstBox->getEntityId());

    {
        CheckUIBox(layoutBox->getEntityId(), center, center);
        CheckUIBox(firstBox->getEntityId(), center, center);
    }

    auto secondBox = createUIBox(0.5f, 0.5f);
    rootLayout->ET_addItem(secondBox->getEntityId());

    {
        CheckUIBox(layoutBox->getEntityId(), center, Vec2i(center.x, viewPort.y));
        CheckUIBox(firstBox->getEntityId(), Vec2i(center.x, 3 * viewPort.y / 4), center);
        CheckUIBox(secondBox->getEntityId(), Vec2i(center.x, viewPort.y / 4), center);
    }
}

TEST_F(UILayoutTests, CheckHiddenElemOnLayout) {
    auto rootEntity = createUIBox(1.f, 1.f);
    auto rootLayout = addUILayout(rootEntity, UILayoutType::Horizontal, UIXAlign::Center, UIYAlign::Center);

    auto childBox = createUIBox(0.5f, 0.5f);
    ET_SendEvent(childBox->getEntityId(), &ETUIElement::ET_hide);

    rootLayout->ET_addItem(childBox->getEntityId());

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    {
        auto box = rootLayout->ET_getCombinedBox();
        auto size = box.getSize();

        EXPECT_EQ(size.x, 0);
        EXPECT_EQ(size.y, 0);
    }

    ET_SendEvent(childBox->getEntityId(), &ETUIElement::ET_show);

    {
        auto box = rootLayout->ET_getCombinedBox();
        auto size = box.getSize();

        EXPECT_EQ(size.x, viewPort.x / 2);
        EXPECT_EQ(size.y, viewPort.y / 2);
    }
}

TEST_F(UILayoutTests, CheckDoNotRebuildLayoutForHiddenHost) {
    auto rootEntity = createUIBox(0.5f, 0.5f);
    auto rootLayout = addUILayout(rootEntity, UILayoutType::Horizontal, UIXAlign::Center, UIYAlign::Center);

    auto leftBox = createUIBox(0.5f, 0.5f);
    rootLayout->ET_addItem(leftBox->getEntityId());

    auto rightBox = createUIBox(0.5f, 0.5f);
    rootLayout->ET_addItem(rightBox->getEntityId());

    ET_SendEvent(rootEntity->getEntityId(), &ETUIElement::ET_hide);

    TestLayoutEventListener layoutEventListener;
    layoutEventListener.listerUpdated(rootLayout->getEntityId());

    {
        UIBoxStyle rootBoxStyle;
        ET_SendEventReturn(rootBoxStyle, rootEntity->getEntityId(), &ETUIBox::ET_getStyle);

        rootBoxStyle.height = 0.25f;
        rootBoxStyle.width = 0.25f;

        ET_SendEvent(rootEntity->getEntityId(), &ETUIBox::ET_setStyle, rootBoxStyle);
    }

    EXPECT_FALSE(layoutEventListener.layoutChanged);

    ET_SendEvent(rootEntity->getEntityId(), &ETUIElement::ET_show);

    EXPECT_TRUE(layoutEventListener.layoutChanged);
}

TEST_F(UILayoutTests, CheckChildLayoutBoxHideUnhide) {

    EntityId firstId;
    EntityId secondId;
    EntityId thirdId;
    {
        auto first = createVoidObject();
        firstId = first->getEntityId();

        auto uiBox = first->addCustomLogic<UIBox>();

        UIBoxStyle boxStyle;
        boxStyle.heightInv = UIBoxSizeInvariant::Relative;
        boxStyle.height = 0.5f;
        boxStyle.widthInv = UIBoxSizeInvariant::Relative;
        boxStyle.width = 0.5f;
        uiBox->ET_setStyle(boxStyle);

        auto uiLayout = first->addCustomLogic<UILayout>();

        UILayoutStyle layoutStyle;
        layoutStyle.xAlign = UIXAlign::Left;
        layoutStyle.yAlign = UIYAlign::Top;
        uiLayout->ET_setStyle(layoutStyle);
    }
    {
        auto second = createVoidObject();
        secondId = second->getEntityId();

        auto uiBox = second->addCustomLogic<UILayoutBox>();
        second->addCustomLogic<UILayout>();
    }
    {
        auto third = createVoidObject();
        thirdId = third->getEntityId();

        auto uiBox = third->addCustomLogic<UIBox>();

        UIBoxStyle style;
        style.heightInv = UIBoxSizeInvariant::Relative;
        style.height = 0.25f;
        style.widthInv = UIBoxSizeInvariant::Relative;
        style.width = 0.25f;
        uiBox->ET_setStyle(style);
    }

    {
        ET_SendEvent(secondId, &ETEntity::ET_addChild, thirdId);
        ET_SendEvent(secondId, &ETUILayout::ET_addItem, thirdId);
    }

    {
        ET_SendEvent(firstId, &ETEntity::ET_addChild, secondId);
        ET_SendEvent(firstId, &ETUILayout::ET_addItem, secondId);
    }

    Vec2i viewport(0);
    ET_SendEventReturn(viewport, &ETUIViewPort::ET_getViewport);
    Vec2i center = viewport / 2;

    {
        Transform tm;
        tm.pt = Vec3(center.x, center.y, 0.f);
        ET_SendEvent(firstId, &ETEntity::ET_setTransform, tm);
    }

    {
        AABB2Di box(0);
        ET_SendEventReturn(box, secondId, &ETUIElement::ET_getBox);
        auto boxSize = box.getSize();
        EXPECT_EQ(boxSize.x, center.x / 2);
        EXPECT_EQ(boxSize.y, center.y / 2);
    }

    ET_SendEvent(firstId, &ETUIElement::ET_hide);
    ET_SendEvent(secondId, &ETUIElement::ET_hide);
    ET_SendEvent(thirdId, &ETUIElement::ET_hide);

    {
        UIBoxStyle boxStyle;
        ET_SendEventReturn(boxStyle, firstId, &ETUIBox::ET_getStyle);
        ET_SendEvent(thirdId, &ETUIBox::ET_setStyle, boxStyle);
    }

    ET_SendEvent(firstId, &ETUIElement::ET_show);
    ET_SendEvent(secondId, &ETUIElement::ET_show);
    ET_SendEvent(thirdId, &ETUIElement::ET_show);

    {
        AABB2Di firstBox(0);
        ET_SendEventReturn(firstBox, firstId, &ETUIElement::ET_getBox);

        AABB2Di secondBox(0);
        ET_SendEventReturn(secondBox, secondId, &ETUIElement::ET_getBox);

        Vec2i firstSize = firstBox.getSize();
        Vec2i secondSize = secondBox.getSize();

        EXPECT_EQ(firstSize.x, secondSize.x);
        EXPECT_EQ(firstSize.y, secondSize.y);

        EXPECT_EQ(firstBox.bot.x, secondBox.bot.x);
        EXPECT_EQ(firstBox.bot.y, secondBox.bot.y);
        EXPECT_EQ(firstBox.top.x, secondBox.top.x);
        EXPECT_EQ(firstBox.top.y, secondBox.top.y);
    }
}