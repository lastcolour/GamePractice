#ifndef __UI_LAYOUT_TESTS_HPP__
#define __UI_LAYOUT_TESTS_HPP__

#include "TestUtils/ConsoleAppTests.hpp"
#include "UI/Logics/UILayout.hpp"

class UILayoutTests : public ConsoleAppTests {
protected:

    Entity* createUIBox(float width, float height);
    UILayout* addUILayout(Entity* entity, UILayoutType type, UIXAlign xAlign, UIYAlign yAlign);
};

#endif /* __UI_LAYOUT_TESTS_HPP__ */