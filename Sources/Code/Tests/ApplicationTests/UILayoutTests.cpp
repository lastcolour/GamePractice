#include "UILayoutTests.hpp"
#include "Logics/UILayout.hpp"
#include "Logics/UIBox.hpp"
#include "UIConfig.hpp"
#include "Core/ETApplication.hpp"

namespace {

void CheckUIBox(EntityId entityId, Vec2i expCenter, Vec2i expSize) {
    AABB2Di box;
    ET_SendEventReturn(box, entityId, &ETUIBox::ET_getBox);

    auto center = box.getCenter();

    EXPECT_EQ(center.x, expCenter.x);
    EXPECT_EQ(center.y, expCenter.y);

    auto size = box.getSize();

    EXPECT_EQ(size.x, expSize.x);
    EXPECT_EQ(size.y, expSize.y);
}

} // namespace

Entity* UILayoutTests::createUIBox(float width, float height) {
    auto entity = createVoidObject();
    std::unique_ptr<UIBox> rootBox(new UIBox);
    auto rootBoxPtr = rootBox.get();
    entity->addCustomLogic(std::move(rootBox));
    UIBoxStyle boxStyle;
    boxStyle.height = width;
    boxStyle.heightInv = UIBoxSizeInvariant::Relative;
    boxStyle.width = height;
    boxStyle.widthInv = UIBoxSizeInvariant::Relative;
    rootBoxPtr->ET_setStyle(boxStyle);

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    Transform tm;
    tm.pt = Vec3(renderPort.x / 2.f, renderPort.y / 2.f, 0.f);
    ET_SendEvent(entity->getEntityId(), &ETEntity::ET_setTransform, tm);

    return entity;
}

UILayout* UILayoutTests::addUILayout(Entity* entity, UILayoutType type, UIXAlign xAlign, UIYAlign yAlign) {
    std::unique_ptr<UILayout> layout(new UILayout);
    auto layoutPtr = layout.get();
    entity->addCustomLogic(std::move(layout));
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

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    {
        AABB2Di box;
        ET_SendEventReturn(box, firstChild->getEntityId(), &ETUIBox::ET_getBox);
        Vec2i expCenter = Vec2i(renderPort.x / 2, 5 * renderPort.y / 8);
        Vec2i resCenter = box.getCenter();
        EXPECT_EQ(resCenter, expCenter);
        Vec2i expSize = renderPort / 4;
        Vec2i resSize = box.getSize();
        EXPECT_EQ(resSize, expSize);
    }
    {
        AABB2Di box;
        ET_SendEventReturn(box, secondChild->getEntityId(), &ETUIBox::ET_getBox);
        Vec2i expCenter = Vec2i(renderPort.x / 2, 3 * renderPort.y / 8);
        Vec2i resCenter = box.getCenter();
        EXPECT_EQ(resCenter, expCenter);
        Vec2i expSize = renderPort / 4;
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

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    {
        AABB2Di box;
        ET_SendEventReturn(box, firstChild->getEntityId(), &ETUIBox::ET_getBox);
        Vec2i expCenter = Vec2i(3 * renderPort.x / 8, renderPort.y / 2);
        Vec2i resCenter = box.getCenter();
        EXPECT_EQ(resCenter, expCenter);
        Vec2i expSize = renderPort / 4;
        Vec2i resSize = box.getSize();
        EXPECT_EQ(resSize, expSize);
    }
    {
        AABB2Di box;
        ET_SendEventReturn(box, secondChild->getEntityId(), &ETUIBox::ET_getBox);
        Vec2i expCenter = Vec2i(5 * renderPort.x / 8, renderPort.y / 2);
        Vec2i resCenter = box.getCenter();
        EXPECT_EQ(resCenter, expCenter);
        Vec2i expSize = renderPort / 4;
        Vec2i resSize = box.getSize();
        EXPECT_EQ(resSize, expSize);
    }
}

TEST_F(UILayoutTests, CheckAlign) {
    auto rootEntity = createUIBox(1.f, 1.f);
    auto rootLayout = addUILayout(rootEntity, UILayoutType::Horizontal, UIXAlign::Center, UIYAlign::Center);

    auto child = createUIBox(0.5f, 0.5f);

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    rootLayout->ET_addItem(child->getEntityId());

    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Center;
        style.yAlign = UIYAlign::Bot;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIBox::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(renderPort.x / 2, renderPort.y / 4);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Center;
        style.yAlign = UIYAlign::Center;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIBox::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(renderPort.x / 2, renderPort.y / 2);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Center;
        style.yAlign = UIYAlign::Top;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIBox::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(renderPort.x / 2, 3 * renderPort.y / 4);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Left;
        style.yAlign = UIYAlign::Bot;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIBox::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(renderPort.x / 4, renderPort.y / 4);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Left;
        style.yAlign = UIYAlign::Center;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIBox::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(renderPort.x / 4, renderPort.y / 2);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Left;
        style.yAlign = UIYAlign::Top;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIBox::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(renderPort.x / 4, 3 * renderPort.y / 4);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Right;
        style.yAlign = UIYAlign::Bot;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIBox::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(3 * renderPort.x / 4, renderPort.y / 4);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Right;
        style.yAlign = UIYAlign::Center;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIBox::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(3 * renderPort.x / 4, renderPort.y / 2);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
    {
        UILayoutStyle style = rootLayout->ET_getStyle();
        style.xAlign = UIXAlign::Right;
        style.yAlign = UIYAlign::Top;
        rootLayout->ET_setStyle(style);

        AABB2Di box;
        ET_SendEventReturn(box, child->getEntityId(), &ETUIBox::ET_getBox);

        auto center = box.getCenter();
        auto expCenter = Vec2i(3 * renderPort.x / 4, 3 * renderPort.y / 4);

        EXPECT_EQ(center.x, expCenter.x);
        EXPECT_EQ(center.y, expCenter.y);
    }
}

TEST_F(UILayoutTests, CheckMargin) {
    auto rootEntity = createUIBox(1.f, 1.f);
    auto rootLayout = addUILayout(rootEntity, UILayoutType::Horizontal, UIXAlign::Center, UIYAlign::Center);

    auto first = createUIBox(0.25f, 0.25f);
    auto second = createUIBox(0.25, 0.25f);

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    const auto boxSize = renderPort / 4;
    const auto center = renderPort / 2;
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