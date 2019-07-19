#include "UIButtonTests.hpp"

#include <ETApplicationInterfaces.hpp>
#include <UI/Logics/UIButton.hpp>
#include <Game/GameObject.hpp>

namespace {

class TestButtonEventListener {
public:

};

} // namespace

void UIButtonTests::SetUp() {
}

void UIButtonTests::TearDown() {
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
    style.size = Vec2(0.5f);
    button->ET_setStyle(style);
    button->init();

    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRender::ET_getRenderPort);
    Vec2i pt = renderPort / 2;

    ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceTouch, ETouchType::Press, pt);
    ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceTouch, ETouchType::Release, pt);
}