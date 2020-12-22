#include "UIAnimationSequenceTests.hpp"
#include "Logics/UIAnimationSequence.hpp"
#include "Logics/UIBox.hpp"
#include "UI/ETUIViewPort.hpp"
#include "Logics/UILayout.hpp"

namespace {

void TickAndApplyAnimation(float dt) {
    ET_SendEvent(&ETUITimerEvents::ET_onUITick, dt);
    ET_SendEvent(&ETUIAdditiveAnimationTarget::ET_applyAdditiveTranform);
}

} // namespace

TEST_F(UIAnimationSequenceTests, CheckInOut) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    auto entity = createVoidObject();
    {
        Transform tm;
        tm.pt = Vec3(viewPort.x / 2.f, viewPort.y / 2.f, 0.f);
        entity->ET_setTransform(tm);
    }
    UIBox* uiBox = nullptr;
    {
        uiBox = entity->addCustomLogic<UIBox>();
        ASSERT_TRUE(uiBox);

        UIBoxStyle style;
        style.width = 0.5f;
        style.height = 0.5f;
        style.heightInv = UIBoxSizeInvariant::Relative;
        style.widthInv = UIBoxSizeInvariant::Relative;
        uiBox->ET_setStyle(style);
    }
    UIAnimationSequence* animSeq = nullptr;
    {
        animSeq = entity->addCustomLogic<UIAnimationSequence>();
        ASSERT_TRUE(animSeq);

        animSeq->ET_setType(EAnimSequenceType::Idle);
        UIAnimationFrame inFrame;
        inFrame.duration = 1.f;
        inFrame.scale = Vec2(0.5f);
        animSeq->ET_addAnimationFrame(inFrame);

        UIAnimationFrame outFrame;
        outFrame.duration = 1.f;
        outFrame.scale = Vec2(1.f);
        animSeq->ET_addAnimationFrame(outFrame);
    }

    animSeq->ET_playAnimation(InvalidEntityId);

    TickAndApplyAnimation(0.5f);

    {
        auto tm = entity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.y, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }

    TickAndApplyAnimation(0.5f);

    {
        auto tm = entity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 0.5f);
        EXPECT_FLOAT_EQ(tm.scale.y, 0.5f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }

    TickAndApplyAnimation(0.5f);

    {
        auto tm = entity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.y, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }

    TickAndApplyAnimation(0.5f);

    {
        auto tm = entity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.y, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }
}

TEST_F(UIAnimationSequenceTests, CheckSubAnimation) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    auto parentEntity = createVoidObject();
    {
        auto uiBox = parentEntity->addCustomLogic<UIBox>();
        ASSERT_TRUE(uiBox);

        auto uiLayout = parentEntity->addCustomLogic<UILayout>();
        ASSERT_TRUE(uiLayout);

        Transform tm;
        tm.pt = Vec3(viewPort.x / 2.f, viewPort.y / 2.f, 0.f);
        parentEntity->ET_setTransform(tm);
    }
    UIAnimationSequence* parentAnimSeq = nullptr;
    {
        parentAnimSeq = parentEntity->addCustomLogic<UIAnimationSequence>();
        ASSERT_TRUE(parentAnimSeq);

        parentAnimSeq->ET_setType(EAnimSequenceType::Press);

        UIAnimationFrame frame;
        frame.duration = 1.f;

        parentAnimSeq->ET_addAnimationFrame(frame);
    }

    auto childEntity = createVoidObject();
    {
        auto uiBox = childEntity->addCustomLogic<UIBox>();
        ASSERT_TRUE(uiBox);
    }

    UIAnimationSequence* childAnimSeq = nullptr;
    {
        childAnimSeq = childEntity->addCustomLogic<UIAnimationSequence>();
        ASSERT_TRUE(childAnimSeq);

        childAnimSeq->ET_setType(EAnimSequenceType::Press);

        UIAnimationFrame frame;
        frame.duration = 1.f;
        frame.scale = Vec2(0.5f);

        childAnimSeq->ET_addAnimationFrame(frame);
    }

    parentEntity->ET_addChild(childEntity->getEntityId());
    ET_SendEvent(parentEntity->getEntityId(), &ETUILayout::ET_addItem, childEntity->getEntityId());

    parentAnimSeq->ET_addSubAnimation(childEntity->getEntityId());
    parentAnimSeq->ET_playAnimation(InvalidEntityId);

    TickAndApplyAnimation(0.5f);

    {
        auto tm = parentEntity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.y, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }
    {
        auto tm = childEntity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.y, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }

    TickAndApplyAnimation(0.4999999f);

    {
        auto tm = parentEntity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.y, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }
    {
        auto tm = childEntity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 0.5f);
        EXPECT_FLOAT_EQ(tm.scale.y, 0.5f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }

    TickAndApplyAnimation(0.01f);

    {
        auto tm = parentEntity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.y, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }
    {
        auto tm = childEntity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.y, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }
}

TEST_F(UIAnimationSequenceTests, CheckAnimationStopInTheMiddle) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    auto entity = createVoidObject();
    {
        Transform tm;
        tm.pt = Vec3(viewPort.x / 2.f, viewPort.y / 2.f, 0.f);
        entity->ET_setTransform(tm);
    }
    UIBox* uiBox = nullptr;
    {
        uiBox = entity->addCustomLogic<UIBox>();
        ASSERT_TRUE(uiBox);

        UIBoxStyle style;
        style.width = 0.5f;
        style.height = 0.5f;
        style.heightInv = UIBoxSizeInvariant::Relative;
        style.widthInv = UIBoxSizeInvariant::Relative;
        uiBox->ET_setStyle(style);

    }
    UIAnimationSequence* animSeq = nullptr;
    {
        animSeq = entity->addCustomLogic<UIAnimationSequence>();
        ASSERT_TRUE(animSeq);

        animSeq->ET_setType(EAnimSequenceType::Idle);
        UIAnimationFrame inFrame;
        inFrame.duration = 1.f;
        inFrame.scale = Vec2(0.5f);
        animSeq->ET_addAnimationFrame(inFrame);
    }

    animSeq->ET_playAnimation(InvalidEntityId);

    TickAndApplyAnimation(0.5f);

    {
        auto tm = entity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.y, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }

    animSeq->ET_stopAnimation();
    TickAndApplyAnimation(0.f);

    {
        auto tm = entity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.y, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }
}

TEST_F(UIAnimationSequenceTests, CheckHideDuringAnimation) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    auto entity = createVoidObject();
    {
        Transform tm;
        tm.pt = Vec3(viewPort.x / 2.f, viewPort.y / 2.f, 0.f);
        entity->ET_setTransform(tm);
    }
    UIBox* uiBox = nullptr;
    {
        uiBox = entity->addCustomLogic<UIBox>();
        ASSERT_TRUE(uiBox);

        UIBoxStyle style;
        style.width = 0.5f;
        style.height = 0.5f;
        style.heightInv = UIBoxSizeInvariant::Relative;
        style.widthInv = UIBoxSizeInvariant::Relative;
        uiBox->ET_setStyle(style);

    }
    UIAnimationSequence* animSeq = nullptr;
    {
        animSeq = entity->addCustomLogic<UIAnimationSequence>();
        ASSERT_TRUE(animSeq);

        animSeq->ET_setType(EAnimSequenceType::Idle);
        UIAnimationFrame inFrame;
        inFrame.duration = 1.f;
        inFrame.scale = Vec2(0.5f);
        animSeq->ET_addAnimationFrame(inFrame);
    }

    animSeq->ET_playAnimation(InvalidEntityId);

    TickAndApplyAnimation(0.5f);

    {
        auto tm = entity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.y, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }

    uiBox->ET_hide();
    TickAndApplyAnimation(0.f);
    uiBox->ET_show();

    {
        auto tm = entity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.y, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }
}

TEST_F(UIAnimationSequenceTests, CheckTwoAnimationAtTheSameTime) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    auto entity = createVoidObject();
    {
        Transform tm;
        tm.pt = Vec3(viewPort.x / 2.f, viewPort.y / 2.f, 0.f);
        entity->ET_setTransform(tm);
    }
    UIBox* uiBox = nullptr;
    {
        uiBox = entity->addCustomLogic<UIBox>();
        ASSERT_TRUE(uiBox);

        UIBoxStyle style;
        style.width = 0.5f;
        style.height = 0.5f;
        style.heightInv = UIBoxSizeInvariant::Relative;
        style.widthInv = UIBoxSizeInvariant::Relative;
        uiBox->ET_setStyle(style);
    }
    UIAnimationSequence* firstAnim = nullptr;
    {
        firstAnim = entity->addCustomLogic<UIAnimationSequence>();
        ASSERT_TRUE(firstAnim);

        firstAnim->ET_setType(EAnimSequenceType::Idle);

        UIAnimationFrame frame;
        frame.duration = 1.f;
        frame.offset = Vec2(100.f, 100.f);
        firstAnim->ET_addAnimationFrame(frame);
    }
    UIAnimationSequence* secondAnim = nullptr;
    {
        secondAnim = entity->addCustomLogic<UIAnimationSequence>();
        ASSERT_TRUE(secondAnim);

        secondAnim->ET_setType(EAnimSequenceType::Idle);

        UIAnimationFrame frame;
        frame.duration = 1.f;
        frame.offset = Vec2(-100.f, -100.f);
        secondAnim->ET_addAnimationFrame(frame);
    }

    firstAnim->ET_playAnimation(InvalidEntityId);
    secondAnim->ET_playAnimation(InvalidEntityId);

    TickAndApplyAnimation(0.5f);

    {
        auto tm = entity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.y, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }

    TickAndApplyAnimation(0.5f);

    {
        auto tm = entity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.y, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }
}

TEST_F(UIAnimationSequenceTests, CheckAnimationRestartWithSubAnimation) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);

    auto parentEntity = createVoidObject();
    {
        Transform tm;
        tm.pt = Vec3(viewPort.x / 2.f, viewPort.y / 2.f, 0.f);
        parentEntity->ET_setTransform(tm);

        auto uiBox = parentEntity->addCustomLogic<UIBox>();
        ASSERT_TRUE(uiBox);

        UIBoxStyle style;
        style.width = 0.5f;
        style.height = 0.5f;
        style.heightInv = UIBoxSizeInvariant::Relative;
        style.widthInv = UIBoxSizeInvariant::Relative;
        uiBox->ET_setStyle(style);

        auto uiLayout = parentEntity->addCustomLogic<UILayout>();
        ASSERT_TRUE(uiLayout);
    }
    UIAnimationSequence* parentAnim = nullptr;
    {
        parentAnim = parentEntity->addCustomLogic<UIAnimationSequence>();
        ASSERT_TRUE(parentAnim);

        parentAnim->ET_setType(EAnimSequenceType::Idle);

        UIAnimationFrame frame;
        frame.duration = 1.f;
        frame.scale = Vec2(1.f);
        parentAnim->ET_setStartEndEvents(EShowEvent::Show, EShowEvent::Hide);
        parentAnim->ET_addAnimationFrame(frame);
    }

    auto childEntity = createVoidObject();
    {
        auto uiBox = childEntity->addCustomLogic<UIBox>();
        ASSERT_TRUE(uiBox);

        UIBoxStyle style;
        style.width = 0.25f;
        style.height = 0.25f;
        style.heightInv = UIBoxSizeInvariant::Relative;
        style.widthInv = UIBoxSizeInvariant::Relative;
        uiBox->ET_setStyle(style);
    }
    UIAnimationSequence* childAnim = nullptr;
    {
        childAnim = childEntity->addCustomLogic<UIAnimationSequence>();
        ASSERT_TRUE(childAnim);

        childAnim->ET_setType(EAnimSequenceType::Idle);

        UIAnimationFrame frame;
        frame.duration = 1.f;
        frame.scale = Vec2(0.5f);
        childAnim->ET_addAnimationFrame(frame);
    }

    parentEntity->ET_addChild(childEntity->getEntityId());
    ET_SendEvent(parentEntity->getEntityId(), &ETUILayout::ET_addItem, childEntity->getEntityId());
    parentAnim->ET_addSubAnimation(childEntity->getEntityId());

    parentAnim->ET_playAnimation(InvalidEntityId);
    TickAndApplyAnimation(0.5f);

    {
        auto tm = childEntity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.y, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);

        bool isHidden = false;
        ET_SendEventReturn(isHidden, childEntity->getEntityId(), &ETUIElement::ET_isHidden);
        EXPECT_FALSE(isHidden);
    }

    TickAndApplyAnimation(0.5f);

    {
        auto tm = childEntity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.y, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);

        bool isHidden = false;
        ET_SendEventReturn(isHidden, childEntity->getEntityId(), &ETUIElement::ET_isHidden);
        EXPECT_TRUE(isHidden);
    }

    parentAnim->ET_playAnimation(InvalidEntityId);
    TickAndApplyAnimation(0.5f);

    {
        auto tm = childEntity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.y, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);

        bool isHidden = false;
        ET_SendEventReturn(isHidden, childEntity->getEntityId(), &ETUIElement::ET_isHidden);
        EXPECT_FALSE(isHidden);
    }

    TickAndApplyAnimation(0.5f);

    {
        auto tm = childEntity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.y, 1.f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);

        bool isHidden = false;
        ET_SendEventReturn(isHidden, childEntity->getEntityId(), &ETUIElement::ET_isHidden);
        EXPECT_TRUE(isHidden);
    }
}