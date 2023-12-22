#ifndef __GAME_DOUBLE_TAP_TESTS_HPP__
#define __GAME_DOUBLE_TAP_TESTS_HPP__

#include "TestUtils/EngineTests.hpp"

class GameDoubleTapTests : public EngineTests {
public:

    static void SetUpTestCase() {
        CreateTestApp(ETestAppModules::CheckRender);
    }
};

#endif /* __GAME_DOUBLE_TAP_TESTS_HPP__ */