#include "UIButtonTests.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIButton.hpp"
#include "Platform/ETSurface.hpp"
#include "Logics/UIButton.hpp"
#include "Logics/UIBox.hpp"
#include "Logics/UIPressAnimation.hpp"
#include "Core/ETApplication.hpp"
#include "Config/UIConfig.hpp"
#include "UI/ETUIViewScript.hpp"

class TestButtonEventListener : public ETNode<ETUIViewScript> {
public:
    TestButtonEventListener() {
        auto entId = GetETSystem()->createNewEntityId();
        ETNode<ETUIViewScript>::connect(entId);
    }

    virtual ~TestButtonEventListener() = default;

    // ETUIEventManager
    void ET_onGetFocus() override {}

    void ET_onLostFocus() override {}

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
    entity->addCustomLogic(std::unique_ptr<EntityLogic>(new UIButton()));
    entity->addCustomLogic(std::unique_ptr<EntityLogic>(new UIBox()));

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

    auto button = createUIButton(center, Vec2(0.5f));

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
    auto topButton = createUIButton(topButtonCenter, Vec2(0.5f));

    Vec2i botButtonCenter = Vec2i(center.x, viewPort.y / 4);
    auto botButton = createUIButton(botButtonCenter, Vec2(0.5f));

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
    auto botButton = createUIButton(botButtonCenter, Vec2(0.5f));

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

    float animDuration = 0.f;

    {
        auto pressAnimation = new UIPressAnimation();
        animDuration = pressAnimation->ET_getDuration();
        ASSERT_GT(animDuration, 0.f);
        button->addCustomLogic(std::unique_ptr<EntityLogic>(pressAnimation));
    }

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, center);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Move, center);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, center);

    ASSERT_EQ(buttonListener->eventQueue.size(), 0u);

    ET_SendEvent(&ETUITimerEvents::ET_onUITick, animDuration + 0.001f);
    ASSERT_EQ(buttonListener->eventQueue.size(), 1u);

    auto& event = buttonListener->eventQueue[0];
    EXPECT_EQ(event.type, UIEvent::EventType::None);
    EXPECT_EQ(event.senderId, button->getEntityId());
}