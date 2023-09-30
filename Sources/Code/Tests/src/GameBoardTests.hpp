#ifndef __GAME_BOARD_TESTS_HPP__
#define __GAME_BOARD_TESTS_HPP__

#include "TestUtils/EngineTests.hpp"

class TestGameBoardLogic;
class Entity;

class GameBoardTests : public EngineTests {
protected:

    static void SetUpTestCase() {
        CreateTestApp(ETestAppModules::CheckGame);
    }

    void SetUp();

protected:

    Entity* entity;
    TestGameBoardLogic* board;
};

#endif /* __GAME_BOARD_TESTS_HPP__ */