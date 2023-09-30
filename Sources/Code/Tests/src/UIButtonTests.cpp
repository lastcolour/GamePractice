#include "UIButtonTests.hpp"
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
        auto entId = GetEnv()->GetETSystem()->createNewEntityId();
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

void UIButtonTests::SetUpTestCase() {
    CreateTestApp(ETestAppModules::CheckUI);

    auto uiConfig = Core::GetGlobal<UIConfig>();
    ASSERT_TRUE(uiConfig);

    uiConfig->baseRatio = Vec2i(1);
    Vec2i portSize(uiConfig->horizontalGrid);

    ET_SendEvent(&ETUIViewPort::ET_setViewPort, portSize);
}

void UIButtonTests::SetUp() {
    EngineTests::SetUp();
    buttonListener.reset(new TestButtonEventListener);
}

void UIButtonTests::TearDown() {
    buttonListener.reset();
    EngineTests::TearDown();
}

Entity* UIButtonTests::createUIButton(const Vec2i& pos, const Vec2& size) {
    auto entity = createVoidObject();
    HACK_ASSERT_TRUE(entity->addCustomLogic<UIButton>());

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

    {
        TouchEvent event;
        event.actionType = EActionType::Press;
        event.pt = center;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);
    }

    {
        TouchEvent event;
        event.actionType = EActionType::Move;
        event.pt = center;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);
    }

    {
        TouchEvent event;
        event.actionType = EActionType::Release;
        event.pt = center;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);
    }

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

    {
        TouchEvent event;
        event.pt = center;
        event.actionType = EActionType::Press;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);
    }

    pt.y = viewPort.y;

    {
        TouchEvent event;
        event.pt.x = center.x;
        event.pt.y = viewPort.y;

        event.actionType = EActionType::Move;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);

        event.actionType = EActionType::Release;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);
    }

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

    {
        TouchEvent event;
        event.pt = topButtonCenter;
        event.actionType = EActionType::Press;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);
    }

    {
        TouchEvent event;
        event.pt = botButtonCenter;
        event.actionType = EActionType::Move;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);
    }

    {
        TouchEvent event;
        event.pt = botButtonCenter;
        event.actionType = EActionType::Release;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);
    }

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

    {
        TouchEvent event;
        event.pt = topButtonCenter;

        event.actionType = EActionType::Press;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);

        event.actionType = EActionType::Release;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);
    }

    {
        TouchEvent event;
        event.pt = botButtonCenter;

        event.actionType = EActionType::Press;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);

        event.actionType = EActionType::Release;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);
    }

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

    {
        TouchEvent event;
        event.pt = center;

        event.actionType = EActionType::Press;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);

        event.actionType = EActionType::Move;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);

        event.actionType = EActionType::Release;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);
    }

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

    ET_SendEvent(&ETUIReAlignManager::ET_doReAlign);

    ET_SendEvent(parent->getEntityId(), &ETUIView::ET_setFocus, false);

    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    Vec2i center = viewPort / 2;

    {
        TouchEvent event;
        event.pt = center;

        event.actionType = EActionType::Press;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);

        event.actionType = EActionType::Move;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);

        event.actionType = EActionType::Release;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);
    }

    EXPECT_EQ(buttonListener->eventQueue.size(), 0u);

    ET_SendEvent(parent->getEntityId(), &ETUIView::ET_setFocus, true);

    {
        TouchEvent event;
        event.pt = center;

        event.actionType = EActionType::Press;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);

        event.actionType = EActionType::Move;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);

        event.actionType = EActionType::Release;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);
    }

    EXPECT_EQ(buttonListener->eventQueue.size(), 1u);
}

TEST_F(UIButtonTests, CheckHiddenButton) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    Vec2i center = viewPort / 2;

    auto button = createUIButton(center, Vec2(0.5f));

    ET_SendEvent(button->getEntityId(), &ETUIElement::ET_hide);

    {
        TouchEvent event;
        event.pt = center;

        event.actionType = EActionType::Press;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);

        event.actionType = EActionType::Move;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);

        event.actionType = EActionType::Release;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);
    }

    ASSERT_TRUE(buttonListener->eventQueue.empty());
}

TEST_F(UIButtonTests, CheckDisabledButton) {
    Vec2i viewPort(0);
    ET_SendEventReturn(viewPort, &ETUIViewPort::ET_getViewport);
    Vec2i center = viewPort / 2;

    auto button = createUIButton(center, Vec2(0.5f));

    ET_SendEvent(button->getEntityId(), &ETUIElement::ET_disable);

    {
        TouchEvent event;
        event.pt = center;

        event.actionType = EActionType::Press;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);

        event.actionType = EActionType::Move;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);

        event.actionType = EActionType::Release;
        ET_SendEvent(&ETInputEvents::ET_onTouch, event);
    }

    ASSERT_TRUE(buttonListener->eventQueue.empty());
}