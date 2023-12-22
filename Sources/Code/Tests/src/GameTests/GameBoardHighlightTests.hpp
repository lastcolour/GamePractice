#ifndef __GAME_BOARD_HIGHLIGHT_TESTS_HPP__
#define __GAME_BOARD_HIGHLIGHT_TESTS_HPP__

#include "TestUtils/EngineTests.hpp"

class GameBoardHighlightTests : public EngineTests {
public:

    static void SetUpTestCase() {
        CreateTestApp(ETestAppModules::CheckGame);
    }
};

#endif /* __GAME_BOARD_HIGHLIGHT_TESTS_HPP__ */