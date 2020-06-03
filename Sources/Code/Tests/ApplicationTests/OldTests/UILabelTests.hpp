#ifndef __UI_LABEL_TESTS_HPP__
#define __UI_LABEL_TESTS_HPP__

#include "TestUtils/ConsoleAppTests.hpp"

class UIList;
class UILabel;

class UILabelTests : public ConsoleAppTests {
protected:

    UILabel* createUILabel();
    UIList* createUIList();

protected:

    std::vector<std::unique_ptr<Entity>> tempObject;
};


#endif /* __UI_LABEL_TESTS_HPP__ */