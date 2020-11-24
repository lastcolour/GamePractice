#include "UIAnimationSequenceTests.hpp"
#include "Logics/UIAnimationSequence.hpp"
#include "Logics/UIBox.hpp"
#include "UI/ETUIViewPort.hpp"

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
        uiBox = new UIBox();
        entity->addCustomLogic(std::unique_ptr<EntityLogic>(uiBox));

        UIBoxStyle style;
        style.width = 0.5f;
        style.height = 0.5f;
        style.heightInv = UIBoxSizeInvariant::Relative;
        style.widthInv = UIBoxSizeInvariant::Relative;
        uiBox->ET_setStyle(style);

    }
    UIAnimationSequence* animSeq = nullptr;
    {
        animSeq = new UIAnimationSequence();
        entity->addCustomLogic(std::unique_ptr<EntityLogic>(animSeq));

        UIAnimationFrame inFrame;
        inFrame.duration = 1.f;
        inFrame.scale = Vec2(0.5f);
        animSeq->ET_addAnimationFrame(inFrame);

        UIAnimationFrame outFrame;
        outFrame.duration = 1.f;
        outFrame.scale = Vec2(1.f);
        animSeq->ET_addAnimationFrame(outFrame);
    }

    EXPECT_TRUE(animSeq->ET_playAnimation(InvalidEntityId, EAnimSequenceType::Default));

    ET_SendEvent(&ETUITimerEvents::ET_onUITick, 0.5f);

    {
        auto tm = entity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.y, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }

    ET_SendEvent(&ETUITimerEvents::ET_onUITick, 0.5f);

    {
        auto tm = entity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 0.5f);
        EXPECT_FLOAT_EQ(tm.scale.y, 0.5f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }

    ET_SendEvent(&ETUITimerEvents::ET_onUITick, 0.5f);

    {
        auto tm = entity->ET_getTransform();

        EXPECT_FLOAT_EQ(tm.pt.x, viewPort.x / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.y, viewPort.y / 2.f);
        EXPECT_FLOAT_EQ(tm.pt.z, 0.f);

        EXPECT_FLOAT_EQ(tm.scale.x, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.y, 0.75f);
        EXPECT_FLOAT_EQ(tm.scale.z, 1.f);
    }

    ET_SendEvent(&ETUITimerEvents::ET_onUITick, 0.5f);

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
        Transform tm;
        tm.pt = Vec3(viewPort.x / 2.f, viewPort.y / 2.f, 0.f);
        parentEntity->ET_setTransform(tm);
    }
    UIAnimationSequence* parentAnimSeq = nullptr;
    {
        parentAnimSeq = new UIAnimationSequence();
        parentEntity->addCustomLogic(std::unique_ptr<EntityLogic>(parentAnimSeq));

        UIAnimationFrame frame;
        frame.duration = 1.f;

        parentAnimSeq->ET_addAnimationFrame(frame);
    }

    auto childEntity = createVoidObject();
    {
        Transform tm;
        tm.pt = Vec3(viewPort.x / 2.f, viewPort.y / 2.f, 0.f);
        childEntity->ET_setTransform(tm);
    }

    UIAnimationSequence* childAnimSeq = nullptr;
    {
        childAnimSeq = new UIAnimationSequence();
        childEntity->addCustomLogic(std::unique_ptr<EntityLogic>(childAnimSeq));

        UIAnimationFrame frame;
        frame.duration = 1.f;
        frame.scale = Vec2(0.5f);

        childAnimSeq->ET_addAnimationFrame(frame);
    }

    parentEntity->ET_addChild(childEntity->getEntityId());
    parentAnimSeq->ET_addSubAnimation(childEntity->getEntityId());

    parentAnimSeq->ET_playAnimation(InvalidEntityId, EAnimSequenceType::Default);

    ET_SendEvent(&ETUITimerEvents::ET_onUITick, 0.5f);

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

    ET_SendEvent(&ETUITimerEvents::ET_onUITick, 0.4999999f);

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

    ET_SendEvent(&ETUITimerEvents::ET_onUITick, 0.01f);

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