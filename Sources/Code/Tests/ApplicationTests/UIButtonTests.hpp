#ifndef __UI_BUTTON_TESTS_HPP__
#define __UI_BUTTON_TESTS_HPP__

#include "TestUtils/ConsoleAppTests.hpp"

class UIButton;

class UIButtonTests : public ConsoleAppTests {
protected:

    void SetUp();
    void TearDown();

    UIButton* createUIButton();

protected:

    std::vector<std::unique_ptr<GameObject>> tempObject;
};


#endif /* __UI_BUTTON_TESTS_HPP__ */