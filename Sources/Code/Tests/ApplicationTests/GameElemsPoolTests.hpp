#ifndef __GAME_ELEMS_POOL_TESTS_HPP__
#define __GAME_ELEMS_POOL_TESTS_HPP__

#include "TestUtils/EngineTests.hpp"

class GameElemsPoolTests : public EngineTests {
public:

    static void SetUpTestCase() {
        CreateTestApp(ETestAppModules::CheckRender);
    }
};

#endif /* __GAME_ELEMS_POOL_TESTS_HPP__ */