#ifndef __UI_LAYOUT_TESTS_HPP__
#define __UI_LAYOUT_TESTS_HPP__

#include "TestUtils/EngineTests.hpp"
#include "Logics/UILayout.hpp"

class UILayoutTests : public EngineTests {
protected:

    static void SetUpTestCase();

protected:

    Entity* createUILayoutBox();
    Entity* createUIBox(float width, float height);
    UILayout* addUILayout(Entity* entity, UILayoutType type, UIXAlign xAlign, UIYAlign yAlign);
};

#endif /* __UI_LAYOUT_TESTS_HPP__ */