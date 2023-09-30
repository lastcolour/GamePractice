#ifndef __UI_VIEW_TESTS_HPP__
#define __UI_VIEW_TESTS_HPP__

#include "TestUtils/EngineTests.hpp"

class UIViewTests : public EngineTests {
public:

    static void SetUpTestCase() {
        CreateTestApp(ETestAppModules::CheckUI);
    }
};

#endif /* __UI_VIEW_TESTS_HPP__ */