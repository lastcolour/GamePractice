#ifndef __GAME_EVENT_SEQUENCE_TESTS_HPP__
#define __GAME_EVENT_SEQUENCE_TESTS_HPP__

#include "TestUtils/EngineTests.hpp"

class GameEventSequenceTests : public EngineTests {
public:

    static void SetUpTestCase() {
        CreateTestApp(ETestAppModules::CheckGame);
    }
};

#endif /* __GAME_EVENT_SEQUENCE_TESTS_HPP__ */