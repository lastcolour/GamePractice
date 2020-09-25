#ifndef __UI_SCROLL_AREA_TESTS_HPP__
#define __UI_SCROLL_AREA_TESTS_HPP__

#include "TestUtils/ConsoleAppTests.hpp"

class UIScrollAreaStyle;
class UIScrollArea;

class UIScrollAreaTests : public ConsoleAppTests {
public:

    struct TestContext {
        Entity* parentEntity;
        Entity* childEntity;
        UIScrollArea* scrollArea;
    };

protected:

    void SetUp();
    TestContext createTestContext(const Vec2i& parentBoxSize, const Vec2i& childBoxSize, const UIScrollAreaStyle& scrollStyle);
};

#endif /* __UI_SCROLL_AREA_TESTS_HPP__ */