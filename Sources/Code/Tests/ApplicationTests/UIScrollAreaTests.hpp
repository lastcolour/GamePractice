#ifndef __UI_SCROLL_AREA_TESTS_HPP__
#define __UI_SCROLL_AREA_TESTS_HPP__

#include "TestUtils/EngineTests.hpp"

class UIScrollAreaStyle;
class UIScrollArea;

class UIScrollAreaTests : public EngineTests {
public:

    struct TestContext {
        Entity* parentEntity;
        Entity* childEntity;
        UIScrollArea* scrollArea;
    };

protected:

    static void SetUpTestCase();

protected:

    TestContext createTestContext(const Vec2& parentBoxSize, const Vec2& childBoxSize, const UIScrollAreaStyle& scrollStyle);
};

#endif /* __UI_SCROLL_AREA_TESTS_HPP__ */