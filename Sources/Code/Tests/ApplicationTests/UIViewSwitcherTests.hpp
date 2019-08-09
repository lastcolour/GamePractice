#ifndef __UI_SWITCHER_TESTS_HPP__
#define __UI_SWITCHER_TESTS_HPP__

#include "TestUtils/ConsoleAppTests.hpp"

class UIBaseBox;

class UISwitcherTests : public ConsoleAppTests {
public:

    UIBaseBox* createView();

protected:

    std::vector<std::unique_ptr<GameObject>> tempObject;

};

#endif /* __UI_SWITCHER_TESTS_HPP__ */