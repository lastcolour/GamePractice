#include "UIButtonTests.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIButton.hpp"
#include "Platform/ETSurface.hpp"
#include "Logics/UIButton.hpp"
#include "Logics/UIBox.hpp"
#include "Logics/UIAnimationSequence.hpp"
#include "Logics/UIAnimationFrame.hpp"
#include "Core/ETApplication.hpp"
#include "Config/UIConfig.hpp"
#include "UI/ETUIViewScript.hpp"
#include "Logics/UIView.hpp"
#include "Logics/UILayout.hpp"

class TestButtonEventListener : public ETNode<ETUIViewScript> {
public:
    TestButtonEventListener() {
        auto entId = GetETSystem()->createNewEntityId();
        ETNode<ETUIViewScript>::connect(entId);
    }

    virtual ~TestButtonEventListener() = default;

    // ETUIEventManager
    void ET_onViewOpened() override {}

    void ET_onViewClosed() override {}

    void ET_onViewGetFocus() override {}

    void ET_onViewLostFocus() override {}

    void ET_onEvent(const UIEvent& event) override {
        eventQueue.push_back(event);
    }

public:

    std::vector<UIEvent> eventQueue;
};

void UIButtonTests::SetUp() {
    ConsoleAppTests::SetUp();
    buttonListener.reset(new TestButtonEventListener);

    auto gridSize = ET_getShared<UIConfig>()->verticalGrid;
    Vec2i portSize(gridSize, gridSize);
    ET_SendEvent(&ETUIViewPort::ET_setViewPort, portSize);
}

void UIButtonTests::TearDown() {
    buttonListener.reset();
    ConsoleAppTests::TearDown();
}

Entity* UIButtonTests::createUIButton(const Vec2i& pos, const Vec2& size) {
    auto entity = createVoidObject();
    HACK_ASSERT_TRUE(entity->addCustomLogic<UIButton>());
    HACK_ASSERT_TRUE(entity->addCustomLogic<UIBox>());

    UIBoxStyle style;
    style.height = size.y;
    style.width = size.x;
    style.heightInv = UIBoxSizeInvariant::Relative;
    style.widthInv = UIBoxSizeInvariant::Relative;
    ET_SendEvent(entity->getEntityId(), &ETUIBox::ET_setStyle, style);

    Transform tm;
    tm.pt = Vec3(static_cast<float>(pos.x), static_cast<float>(pos.y), 0.f);
    entity->ET_setTransform(tm);

    return entity;
}

TEST_F(UIButtonTests, CheckTouchInside) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    Vec2i center = viewPort / 2;

    auto button = createUIButton(center, Vec2(0.5f));

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, center);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Move, center);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, center);

    ASSERT_EQ(buttonListener->eventQueue.size(), 1u);

    auto& event = buttonListener->eventQueue[0];
    EXPECT_EQ(event.type, UIEvent::EventType::None);
    EXPECT_EQ(event.senderId, button->getEntityId());
}

TEST_F(UIButtonTests, CheckTouchMoveRelease) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    Vec2i center = viewPort / 2;

    createUIButton(center, Vec2(0.5f));

    Vec2i pt = center;

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, pt);

    pt.y = viewPort.y;

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Move, pt);
    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, pt);

    ASSERT_TRUE(buttonListener->eventQueue.empty());
}

TEST_F(UIButtonTests, CheckTwoButtonsTouchMoveRelease) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    Vec2i center = viewPort / 2;

    Vec2i topButtonCenter = Vec2i(center.x, 3 * viewPort.y / 4);
    createUIButton(topButtonCenter, Vec2(0.5f));

    Vec2i botButtonCenter = Vec2i(center.x, viewPort.y / 4);
    createUIButton(botButtonCenter, Vec2(0.5f));

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, topButtonCenter);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Move, botButtonCenter);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, botButtonCenter);

    ASSERT_TRUE(buttonListener->eventQueue.empty());
}

TEST_F(UIButtonTests, CheckPressTwoButtonsAtTheSameTime) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    Vec2i center = viewPort / 2;

    Vec2i topButtonCenter = Vec2i(center.x, 3 * viewPort.y / 4);
    auto topButton = createUIButton(topButtonCenter, Vec2(0.5f));

    Vec2i botButtonCenter = Vec2i(center.x, viewPort.y / 4);
    createUIButton(botButtonCenter, Vec2(0.5f));

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, topButtonCenter);
    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, topButtonCenter);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, botButtonCenter);
    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, botButtonCenter);

    ASSERT_EQ(buttonListener->eventQueue.size(), 2u);

    auto& event = buttonListener->eventQueue[0];
    EXPECT_EQ(event.type, UIEvent::EventType::None);
    EXPECT_EQ(event.senderId, topButton->getEntityId());
}

TEST_F(UIButtonTests, CheckButtonPressAnimation) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    Vec2i center = viewPort / 2;

    auto button = createUIButton(center, Vec2(0.5f));

    {
        auto pressAnimation = button->addCustomLogic<UIAnimationSequence>();
        ASSERT_TRUE(pressAnimation);

        UIAnimationFrame frame;
        frame.duration = 1.f;
        pressAnimation->ET_addAnimationFrame(frame);
        pressAnimation->ET_setType(EAnimSequenceType::Press);
    }

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, center);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Move, center);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, center);

    ASSERT_EQ(buttonListener->eventQueue.size(), 0u);

    ET_SendEvent(&ETUITimerEvents::ET_onUITick, 1.f);
    ASSERT_EQ(buttonListener->eventQueue.size(), 1u);

    auto& event = buttonListener->eventQueue[0];
    EXPECT_EQ(event.type, UIEvent::EventType::None);
    EXPECT_EQ(event.senderId, button->getEntityId());
}

TEST_F(UIButtonTests, CheckButtonFromNonFocusedView) {
    auto parent = createVoidObject();
    {
        ASSERT_TRUE(parent->addCustomLogic<UIView>());
        ASSERT_TRUE(parent->addCustomLogic<UILayout>());
    }
    auto child = createVoidObject();
    {
        ASSERT_TRUE(child->addCustomLogic<UIButton>());
        
        UIBoxStyle boxStyle;
        boxStyle.widthInv = UIBoxSizeInvariant::Relative;
        boxStyle.width = 0.5f;
        boxStyle.heightInv = UIBoxSizeInvariant::Relative;
        boxStyle.height = 0.5f;

        ET_SendEvent(child->getEntityId(), &ETUIBox::ET_setStyle, boxStyle);
    }

    ET_SendEvent(parent->getEntityId(), &ETUILayout::ET_addItem, child->getEntityId());

    ET_SendEvent(parent->getEntityId(), &ETUIView::ET_setFocus, false);

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    Vec2i center = viewPort / 2;

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, center);
    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Move, center);
    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, center);

    EXPECT_EQ(buttonListener->eventQueue.size(), 0u);

    ET_SendEvent(parent->getEntityId(), &ETUIView::ET_setFocus, true);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, center);
    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Move, center);
    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, center);

    EXPECT_EQ(buttonListener->eventQueue.size(), 1u);
}

TEST_F(UIButtonTests, CheckHiddenButton) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    Vec2i center = viewPort / 2;

    auto button = createUIButton(center, Vec2(0.5f));

    ET_SendEvent(button->getEntityId(), &ETUIElement::ET_hide);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, center);
    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Move, center);
    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, center);

    ASSERT_TRUE(buttonListener->eventQueue.empty());
}

TEST_F(UIButtonTests, CheckDisabledButton) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    Vec2i center = viewPort / 2;

    auto button = createUIButton(center, Vec2(0.5f));

    ET_SendEvent(button->getEntityId(), &ETUIElement::ET_disable);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, center);
    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Move, center);
    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, center);

    ASSERT_TRUE(buttonListener->eventQueue.empty());
}