#ifndef __UI_BUTTON_TESTS_HPP__
#define __UI_BUTTON_TESTS_HPP__

#include "TestUtils/ConsoleAppTests.hpp"

class UIButton;
class TestButtonEventListener;

class UIButtonTests : public ConsoleAppTests {
protected:

    static void SetUpTestCase();

protected:

    void SetUp();
    void TearDown();

protected:

    Entity* createUIButton(const Vec2i& pos, const Vec2& size);

protected:

    std::unique_ptr<TestButtonEventListener> buttonListener;
};

#endif /* __UI_BUTTON_TESTS_HPP__ */