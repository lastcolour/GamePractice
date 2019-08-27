#ifndef __UI_BUTTON_TESTS_HPP__
#define __UI_BUTTON_TESTS_HPP__

#include "TestUtils/ConsoleAppTests.hpp"

class UIButton;
class TestButtonEventListener;

class UIButtonTests : public ConsoleAppTests {
protected:

    void SetUp();
    void TearDown();

    UIButton* createUIButton();

protected:

    std::unique_ptr<TestButtonEventListener> buttonListener;
    std::vector<std::unique_ptr<Entity>> tempObject;
};


#endif /* __UI_BUTTON_TESTS_HPP__ */