#include "UIButtonTests.hpp"

#include <ETApplicationInterfaces.hpp>
#include <UI/Logics/UIButton.hpp>
#include <Game/GameObject.hpp>

class TestButtonEventListener : public ETNode<ETUIEventManager> {
public:
    TestButtonEventListener() {
        auto entId = GetETSystem()->createNewEntityId();
        ETNode<ETUIEventManager>::connect(entId);
    }

    virtual ~TestButtonEventListener() = default;

    // ETUIEventManager
    void ET_onEvent(const std::string& eventName) override {
        eventQueue.push_back(eventName);
    }

public:

    std::vector<std::string> eventQueue;
};

void UIButtonTests::SetUp() {
    buttonListener.reset(new TestButtonEventListener);
}

void UIButtonTests::TearDown() {
    buttonListener.reset();
}

UIButton* UIButtonTests::createUIButton() {
    auto object = createVoidObject();
    std::unique_ptr<UIButton> uiButtonPtr(new UIButton);
    UIButton* uiButton = uiButtonPtr.get();
    object->addLogic(std::move(uiButtonPtr));
    tempObject.push_back(std::move(object));
    return uiButton;
}

TEST_F(UIButtonTests, CheckTouchInside) {
    auto button = createUIButton();

    UIStyle style;
    style.sizeInv = SizeInvariant::Relative;
    style.size = Vec2(0.5f);
    button->ET_setStyle(style);
    button->ET_setEventName("testEvent");
    button->init();

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRender::ET_getRenderPort);
    Vec2i pt = renderPort / 2;

    ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceTouch, ETouchType::Press, pt);

    bool isHovered = false;
    ET_SendEventReturn(isHovered, button->getEntityId(), &ETUIButton::ET_isHovered);
    ASSERT_TRUE(isHovered);

    ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceTouch, ETouchType::Release, pt);

    ASSERT_EQ(buttonListener->eventQueue.size(), 1u);
    ASSERT_STREQ(buttonListener->eventQueue[0].c_str(), "testEvent");
}

TEST_F(UIButtonTests, CheckTouchMoveRelease) {
    auto button = createUIButton();

    UIStyle style;
    style.sizeInv = SizeInvariant::Relative;
    style.size = Vec2(0.5f);
    button->ET_setStyle(style);
    button->ET_setEventName("testEvent");
    button->init();

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRender::ET_getRenderPort);
    Vec2i pt = renderPort / 2;

    ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceTouch, ETouchType::Press, pt);

    pt.y = renderPort.y;

    ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceTouch, ETouchType::Move, pt);
    ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceTouch, ETouchType::Release, pt);

    ASSERT_TRUE(buttonListener->eventQueue.empty());
}

TEST_F(UIButtonTests, CheckTwoButtonsTouchMoveRelease) {
    UIStyle style;
    style.sizeInv = SizeInvariant::Relative;
    style.size = Vec2(0.5f);

    auto topButton = createUIButton();
    style.yAlignType = YAlignType::Top;
    topButton->ET_setStyle(style);
    topButton->ET_setEventName("topButton");
    topButton->init();

    auto botButton = createUIButton();
    style.yAlignType = YAlignType::Bot;
    botButton->ET_setStyle(style);
    botButton->ET_setEventName("botButton");
    botButton->init();

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRender::ET_getRenderPort);

    Vec2i pt = Vec2i(renderPort.x / 2, renderPort.y / 4);
    ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceTouch, ETouchType::Press, pt);

    bool isHovered = false;
    ET_SendEventReturn(isHovered, botButton->getEntityId(), &ETUIButton::ET_isHovered);
    ASSERT_TRUE(isHovered);

    pt.y = renderPort.y * 3 / 4;
    ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceTouch, ETouchType::Move, pt);

    isHovered = false;
    ET_SendEventReturn(isHovered, topButton->getEntityId(), &ETUIButton::ET_isHovered);
    ASSERT_TRUE(isHovered);

    ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceTouch, ETouchType::Release, pt);

    ASSERT_TRUE(buttonListener->eventQueue.empty());
}