#ifndef __UI_LABEL_TESTS_HPP__
#define __UI_LABEL_TESTS_HPP__

#include "TestUtils/ConsoleAppTests.hpp"

class UILabel;

class UILabelTests : public ConsoleAppTests {
protected:

    UILabel* createUILabel();

protected:

    std::vector<std::unique_ptr<GameObject>> tempObject;
};


#endif /* __UI_LABEL_TESTS_HPP__ */