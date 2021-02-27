#include "UIScrollAreaTests.hpp"
#include "Logics/UIScrollArea.hpp"
#include "Logics/UIBox.hpp"
#include "Logics/UILayout.hpp"
#include "Logics/UILayoutBox.hpp"
#include "Logics/UIScrollFocus.hpp"
#include "UI/ETUIViewPort.hpp"
#include "Config/UIConfig.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Core/ETApplication.hpp"

namespace {

void SubmitEventSequence(UIScrollAreaTests::TestContext& ctx, const std::vector<Vec2>& sequence) {
    AABB2Di box;
    ET_SendEventReturn(box, ctx.parentEntity->getEntityId(), &ETUIElement::ET_getBox);
    auto boxSize = box.getSize();

    auto tm = ctx.parentEntity->ET_getTransform();

    Vec2i eventPt;

    auto& firstEvent = sequence.front();
    eventPt.x = static_cast<int>(tm.pt.x + boxSize.x * firstEvent.x);
    eventPt.y = static_cast<int>(tm.pt.y + boxSize.y * firstEvent.y);

    ctx.scrollArea->ET_onInputEvent(EActionType::Press, eventPt);

    for(size_t i = 1, sz = sequence.size() - 1; i < sz; ++i) {
        auto& event = sequence[i];
        eventPt.x = static_cast<int>(tm.pt.x + boxSize.x * event.x);
        eventPt.y = static_cast<int>(tm.pt.y + boxSize.y * event.y);

        ctx.scrollArea->ET_onInputEvent(EActionType::Move, eventPt);
    }

    auto& lastEvent = sequence.back();
    eventPt.x = static_cast<int>(tm.pt.x + boxSize.x * lastEvent.x);
    eventPt.y = static_cast<int>(tm.pt.y + boxSize.y * lastEvent.y);

    ctx.scrollArea->ET_onInputEvent(EActionType::Release, eventPt);

    ET_SendEvent(&ETUITimerEvents::ET_onUITick, 0.5f);
}

} // namespace

UIScrollAreaTests::TestContext UIScrollAreaTests::createTestContext(
    const Vec2i& parentBoxSize, const Vec2i& childBoxSize, const UIScrollAreaStyle& scrollStyle) {

    TestContext ctx;
    {
        auto parentEntity = createVoidObject();
        ctx.parentEntity = parentEntity;

        UIBox* parentBox = ctx.parentEntity->addCustomLogic<UIBox>();
        HACK_ASSERT_TRUE(parentBox);

        UIBoxStyle boxStyle;
        boxStyle.height = static_cast<float>(parentBoxSize.x);
        boxStyle.heightInv = UIBoxSizeInvariant::Grid;
        boxStyle.width = static_cast<float>(parentBoxSize.y);
        boxStyle.widthInv = UIBoxSizeInvariant::Grid;
        parentBox->ET_setStyle(boxStyle);

        Transform tm;
        tm.pt = Vec3(17.f);
        parentEntity->ET_setTransform(tm);
    }

    {
        auto childEntity = createVoidObject();
        ctx.childEntity =  childEntity;

        UIBox* childBox =  childEntity->addCustomLogic<UIBox>();
        bool logicExists = childBox != nullptr;
        HACK_ASSERT_TRUE(logicExists);

        UIBoxStyle boxStyle;
        boxStyle.height = static_cast<float>(childBoxSize.x);
        boxStyle.heightInv = UIBoxSizeInvariant::Grid;
        boxStyle.width = static_cast<float>(childBoxSize.y);
        boxStyle.widthInv = UIBoxSizeInvariant::Grid;

        childBox->ET_setStyle(boxStyle);

        ctx.parentEntity->ET_addChild(childBox->getEntityId());
    }

    {
        ctx.scrollArea = ctx.parentEntity->addCustomLogic<UIScrollArea>();
        bool logicExists = ctx.scrollArea != nullptr;
        HACK_ASSERT_TRUE(logicExists);

        ctx.scrollArea->ET_enableKinematicScroll(false);
        ctx.scrollArea->ET_setStyle(scrollStyle);
        ctx.scrollArea->ET_setTarget(ctx.childEntity->getEntityId());
    }

    return ctx;
}

void UIScrollAreaTests::SetUp() {
    ConsoleAppTests::SetUp();
    auto gridSize = ET_getShared<UIConfig>()->horizontalGrid;
    Vec2i portSize(gridSize, gridSize);
    ET_SendEvent(&ETRenderCameraEvents::ET_onRenderPortResized, portSize);
}

TEST_F(UIScrollAreaTests, CheckInitialPosition) {
    auto parentBox = Vec2i(100);
    auto childBox = Vec2i(300);
    auto ctx = createTestContext(parentBox, childBox, UIScrollAreaStyle{});

    {
        UIScrollAreaStyle scrollStyle;
        scrollStyle.origin = UIScrollOrigin::Start;
        scrollStyle.type = UIScrollType::Horizontal;

        ctx.scrollArea->ET_setStyle(scrollStyle);

        Vec2i excpetectPt(0);
        excpetectPt.x = (parentBox.x - childBox.x) / 2;
        excpetectPt.y = 0;

        auto tm = ctx.childEntity->ET_getLocalTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, static_cast<float>(excpetectPt.x));
        EXPECT_FLOAT_EQ(tm.pt.y, static_cast<float>(excpetectPt.y));
    }

    {
        UIScrollAreaStyle scrollStyle;
        scrollStyle.origin = UIScrollOrigin::End;
        scrollStyle.type = UIScrollType::Horizontal;

        ctx.scrollArea->ET_setStyle(scrollStyle);

        Vec2i excpetectPt(0);
        excpetectPt.x = (-parentBox.x + childBox.x) / 2;
        excpetectPt.y = 0;

        auto tm = ctx.childEntity->ET_getLocalTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, static_cast<float>(excpetectPt.x));
        EXPECT_FLOAT_EQ(tm.pt.y, static_cast<float>(excpetectPt.y));
    }

    {
        UIScrollAreaStyle scrollStyle;
        scrollStyle.origin = UIScrollOrigin::Start;
        scrollStyle.type = UIScrollType::Vertical;

        ctx.scrollArea->ET_setStyle(scrollStyle);

        Vec2i excpetectPt(0);
        excpetectPt.x = 0;
        excpetectPt.y = (parentBox.y - childBox.y) / 2;

        auto tm = ctx.childEntity->ET_getLocalTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, static_cast<float>(excpetectPt.x));
        EXPECT_FLOAT_EQ(tm.pt.y, static_cast<float>(excpetectPt.y));
    }

    {
        UIScrollAreaStyle scrollStyle;
        scrollStyle.origin = UIScrollOrigin::End;
        scrollStyle.type = UIScrollType::Vertical;

        ctx.scrollArea->ET_setStyle(scrollStyle);

        Vec2i excpetectPt(0);
        excpetectPt.x = 0;
        excpetectPt.y = (childBox.y - parentBox.y) / 2;

        auto tm = ctx.childEntity->ET_getLocalTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, static_cast<float>(excpetectPt.x));
        EXPECT_FLOAT_EQ(tm.pt.y, static_cast<float>(excpetectPt.y));
    }
}

TEST_F(UIScrollAreaTests, CheckZeroScroll) {
    auto parentBox = Vec2i(100);
    auto childBox = Vec2i(200);

    UIScrollAreaStyle style;
    style.origin = UIScrollOrigin::Start;
    style.type = UIScrollType::Vertical;
    auto ctx = createTestContext(parentBox, childBox, style);

    auto childOrigTm = ctx.childEntity->ET_getTransform();

    SubmitEventSequence(ctx, {Vec2(0.f, 0.f), Vec2(0.f, 0.f), Vec2(0.f, 0.f)});

    auto childCurrTm = ctx.childEntity->ET_getTransform();

    auto shift = childCurrTm.pt - childOrigTm.pt;

    EXPECT_FLOAT_EQ(shift.x, 0.f);
    EXPECT_FLOAT_EQ(shift.y, 0.f);
}

TEST_F(UIScrollAreaTests, CheckSimpleVerticalScroll) {
    auto parentBox = Vec2i(100);
    auto childBox = Vec2i(200);

    UIScrollAreaStyle style;
    style.origin = UIScrollOrigin::Start;
    style.type = UIScrollType::Vertical;
    auto ctx = createTestContext(parentBox, childBox, style);

    auto childOrigTm = ctx.childEntity->ET_getTransform();

    SubmitEventSequence(ctx, {Vec2(0.f, -0.25f), Vec2(0.f, 0.f), Vec2(0.f, 0.25f)});

    auto childCurrTm = ctx.childEntity->ET_getTransform();

    auto shift = childCurrTm.pt - childOrigTm.pt;
    Vec2 expectedShift(0.f);
    expectedShift.y = parentBox.y * 0.5f;

    EXPECT_FLOAT_EQ(shift.x, expectedShift.x);
    EXPECT_FLOAT_EQ(shift.y, expectedShift.y);
}

TEST_F(UIScrollAreaTests, CheckSimpleHorizontalScroll) {
    auto parentBox = Vec2i(100);
    auto childBox = Vec2i(200);

    UIScrollAreaStyle style;
    style.origin = UIScrollOrigin::Start;
    style.type = UIScrollType::Horizontal;
    auto ctx = createTestContext(parentBox, childBox, style);

    auto childOrigTm = ctx.childEntity->ET_getTransform();

    SubmitEventSequence(ctx, {Vec2(-0.25f, 0.f), Vec2(0.f, 0.f), Vec2(0.25f, 0.f)});

    auto childCurrTm = ctx.childEntity->ET_getTransform();

    auto shift = childCurrTm.pt - childOrigTm.pt;
    Vec2 expectedShift(0.f);
    expectedShift.x = parentBox.y * 0.5f;

    EXPECT_FLOAT_EQ(shift.x, expectedShift.x);
    EXPECT_FLOAT_EQ(shift.y, expectedShift.y);
}

TEST_F(UIScrollAreaTests, CheckIngoreVerticalOnHorizontal) {
    auto parentBox = Vec2i(100);
    auto childBox = Vec2i(200);

    UIScrollAreaStyle style;
    style.origin = UIScrollOrigin::Start;
    style.type = UIScrollType::Horizontal;
    auto ctx = createTestContext(parentBox, childBox, style);

    auto childOrigTm = ctx.childEntity->ET_getTransform();

    SubmitEventSequence(ctx, {Vec2(0.f, -0.25f), Vec2(0.f, 0.f), Vec2(0.f, 0.25f)});

    auto childCurrTm = ctx.childEntity->ET_getTransform();

    auto shift = childCurrTm.pt - childOrigTm.pt;
    Vec2 expectedShift(0.f);

    EXPECT_FLOAT_EQ(shift.x, expectedShift.x);
    EXPECT_FLOAT_EQ(shift.y, expectedShift.y);
}

TEST_F(UIScrollAreaTests, CheckIngoreHorizontalOnVertical) {
    auto parentBox = Vec2i(100);
    auto childBox = Vec2i(200);

    UIScrollAreaStyle style;
    style.origin = UIScrollOrigin::Start;
    style.type = UIScrollType::Vertical;
    auto ctx = createTestContext(parentBox, childBox, style);

    auto childOrigTm = ctx.childEntity->ET_getTransform();

    SubmitEventSequence(ctx, {Vec2(-0.25f, 0.f), Vec2(0.f, 0.f), Vec2(0.25f, 0.0f)});

    auto childCurrTm = ctx.childEntity->ET_getTransform();

    auto shift = childCurrTm.pt - childOrigTm.pt;
    Vec2 expectedShift(0.f);

    EXPECT_FLOAT_EQ(shift.x, expectedShift.x);
    EXPECT_FLOAT_EQ(shift.y, expectedShift.y);
}

TEST_F(UIScrollAreaTests, CheckScrollLimits) {
    auto parentBox = Vec2i(100);
    auto childBox = Vec2i(110);

    UIScrollAreaStyle style;
    style.origin = UIScrollOrigin::Start;
    style.type = UIScrollType::Vertical;
    auto ctx = createTestContext(parentBox, childBox, style);

    auto childOrigTm = ctx.childEntity->ET_getTransform();

    SubmitEventSequence(ctx, {Vec2(0.f, -0.75f), Vec2(0.f, 0.f), Vec2(0.f, 0.75f)});

    Vec2 expectedShift(0.f);
    expectedShift.y = static_cast<float>(childBox.y - parentBox.y);

    auto childCurrTm = ctx.childEntity->ET_getTransform();
    auto shift = childCurrTm.pt - childOrigTm.pt;

    EXPECT_FLOAT_EQ(shift.x, expectedShift.x);
    EXPECT_FLOAT_EQ(shift.y, expectedShift.y);
}

TEST_F(UIScrollAreaTests, CheckIgnoreScrollForSmallerChild) {
    auto parentBox = Vec2i(100);
    auto childBox = Vec2i(50);

    UIScrollAreaStyle style;
    style.origin = UIScrollOrigin::Start;
    style.type = UIScrollType::Vertical;
    auto ctx = createTestContext(parentBox, childBox, style);

    auto childOrigTm = ctx.childEntity->ET_getTransform();

    SubmitEventSequence(ctx, {Vec2(0.f, -0.75f), Vec2(0.f, 0.f), Vec2(0.f, 0.75f)});

    Vec2 expectedShift(0.f);

    auto childCurrTm = ctx.childEntity->ET_getTransform();
    auto shift = childCurrTm.pt - childOrigTm.pt;

    EXPECT_FLOAT_EQ(shift.x, expectedShift.x);
    EXPECT_FLOAT_EQ(shift.y, expectedShift.y);
}

TEST_F(UIScrollAreaTests, CheckScrollProgress) {
    auto parentBox = Vec2i(50);
    auto childBox = Vec2i(100);

    UIScrollAreaStyle style;
    style.origin = UIScrollOrigin::Start;
    style.type = UIScrollType::Vertical;
    auto ctx = createTestContext(parentBox, childBox, style);

    {
        auto scrollProg = ctx.scrollArea->ET_getScrollProgress();
        EXPECT_FLOAT_EQ(scrollProg, 0.f);
    }

    Transform tm = ctx.childEntity->ET_getTransform();
    Vec2i center(tm.pt.x, tm.pt.y);
    center += childBox / 4;
    ctx.scrollArea->ET_setTargetPosClamped(center);

    {
        auto scrollProg = ctx.scrollArea->ET_getScrollProgress();
        EXPECT_FLOAT_EQ(scrollProg, 0.5f);
    }

    Vec2i topPt = center + childBox / 4;
    ctx.scrollArea->ET_setTargetPosClamped(topPt);

    {
        auto scrollProg = ctx.scrollArea->ET_getScrollProgress();
        EXPECT_FLOAT_EQ(scrollProg, 1.f);
    }
}

TEST_F(UIScrollAreaTests, CheckSetFocusToEntity) {
    EntityId firstId;
    EntityId secondId;
    std::vector<EntityId> childIds;
    const int childCount = 3;

    {
        auto first = createVoidObject();
        firstId = first->getEntityId();

        auto uiBox = first->addCustomLogic<UIBox>();

        UIBoxStyle boxStyle;
        boxStyle.widthInv = UIBoxSizeInvariant::Grid;
        boxStyle.width = 50.f;
        boxStyle.heightInv = UIBoxSizeInvariant::Grid;
        boxStyle.height = 50.f;
        uiBox->ET_setStyle(boxStyle);

        auto uiScrollArea = first->addCustomLogic<UIScrollArea>();

        UIScrollAreaStyle scrollStyle;
        scrollStyle.type = UIScrollType::Vertical;
        scrollStyle.origin = UIScrollOrigin::Start;
        uiScrollArea->ET_setStyle(scrollStyle);

        first->addCustomLogic<UIScrollFocus>();
    }
    {
        auto second = createVoidObject();
        secondId = second->getEntityId();

        second->addCustomLogic<UILayoutBox>();

        auto uiLayout = second->addCustomLogic<UILayout>();

        UILayoutStyle style;
        style.type = UILayoutType::Vertical;
        style.xAlign = UIXAlign::Center;
        style.yAlign = UIYAlign::Center;

        uiLayout->ET_setStyle(style);
    }

    {
        for(int i = 0; i < childCount; ++i) {
            auto child = createVoidObject();
            childIds.push_back(child->getEntityId());

            auto uiBox = child->addCustomLogic<UIBox>();

            UIBoxStyle boxStyle;
            boxStyle.widthInv = UIBoxSizeInvariant::Grid;
            boxStyle.width = 50.f;
            boxStyle.heightInv = UIBoxSizeInvariant::Grid;
            boxStyle.height = 50.f;
            uiBox->ET_setStyle(boxStyle);
        }
    }

    {
        for(auto childId : childIds) {
            ET_SendEvent(secondId, &ETEntity::ET_addChild, childId);
            ET_SendEvent(secondId, &ETUILayout::ET_addItem, childId);
        }
        ET_SendEvent(firstId, &ETEntity::ET_addChild, secondId);
        ET_SendEvent(firstId, &ETUIScrollArea::ET_setTarget, secondId);
    }

    for(int i = 0; i < childCount; ++i) {
        auto childId = childIds[i];
        ET_SendEvent(firstId, &ETUIScrollFocus::ET_setFocusToEntity, childId);

        Transform childTm;
        ET_SendEventReturn(childTm, childId, &ETEntity::ET_getTransform);

        Transform firstTm;
        ET_SendEventReturn(firstTm, firstId, &ETEntity::ET_getTransform);

        EXPECT_FLOAT_EQ(childTm.pt.x, firstTm.pt.x);
        EXPECT_FLOAT_EQ(childTm.pt.y, firstTm.pt.y);
    }
}