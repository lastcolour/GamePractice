#include "UIButtonTests.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIButton.hpp"
#include "UI/ETUIView.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Platform/ETSurface.hpp"

class TestButtonEventListener : public ETNode<ETUIEventManager> {
public:
    TestButtonEventListener() {
        auto entId = GetETSystem()->createNewEntityId();
        ETNode<ETUIEventManager>::connect(entId);
    }

    virtual ~TestButtonEventListener() = default;

    // ETUIEventManager
    void ET_onEvent(UIEventType eventType) override {
        eventQueue.push_back(eventType);
    }

public:

    std::vector<UIEventType> eventQueue;
};

void UIButtonTests::SetUp() {
    buttonListener.reset(new TestButtonEventListener);
}

void UIButtonTests::TearDown() {
    buttonListener.reset();
}

Entity* UIButtonTests::createUIButton(const Vec2i& pos, const Vec2i& size) {
    return nullptr;
}

TEST_F(UIButtonTests, CheckTouchInside) {
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);
    Vec2i center = renderPort / 2;

    auto button = createUIButton(center, renderPort / 2);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, center);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Move, center);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, center);

    ASSERT_EQ(buttonListener->eventQueue.size(), 1u);
    ASSERT_EQ(buttonListener->eventQueue[0], UIEventType::None);
}

TEST_F(UIButtonTests, CheckTouchMoveRelease) {
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);
    Vec2i center = renderPort / 2;

    auto button = createUIButton(center, renderPort / 2);

    Vec2i pt = center;

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, pt);

    pt.y = renderPort.y;

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Move, pt);
    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, pt);

    ASSERT_TRUE(buttonListener->eventQueue.empty());
}

TEST_F(UIButtonTests, CheckTwoButtonsTouchMoveRelease) {
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);
    Vec2i center = renderPort / 2;

    Vec2i topButtonCenter = Vec2i(center.x, 3 * renderPort.y / 4);
    auto topButton = createUIButton(topButtonCenter, renderPort / 2);

    Vec2i botButtonCenter = Vec2i(center.x, renderPort.y / 4);
    auto botButton = createUIButton(botButtonCenter, renderPort / 2);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, topButtonCenter);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Move, botButtonCenter);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, botButtonCenter);

    ASSERT_TRUE(buttonListener->eventQueue.empty());
}

TEST_F(UIButtonTests, CheckPressTwoButtonsAtTheSameTime) {
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);
    Vec2i center = renderPort / 2;

    Vec2i topButtonCenter = Vec2i(center.x, 3 * renderPort.y / 4);
    auto topButton = createUIButton(topButtonCenter, renderPort / 2);

    Vec2i botButtonCenter = Vec2i(center.x, renderPort.y / 4);
    auto botButton = createUIButton(botButtonCenter, renderPort / 2);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, topButtonCenter);
    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, topButtonCenter);

    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, botButtonCenter);
    ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, botButtonCenter);

    ASSERT_EQ(buttonListener->eventQueue.size(), 1u);
    ASSERT_EQ(buttonListener->eventQueue[0], UIEventType::None);
}