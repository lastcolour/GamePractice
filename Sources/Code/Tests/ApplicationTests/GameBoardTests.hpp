#ifndef __GAME_BOARD_TESTS_HPP__
#define __GAME_BOARD_TESTS_HPP__

#include "TestUtils/ConsoleAppTests.hpp"

class TestGameBoardLogic;
class Entity;

class GameBoardTests : public ConsoleAppTests {
protected:

    void SetUp();
    void TearDown();

protected:

    std::unique_ptr<Entity> entity;
    TestGameBoardLogic* board;
};

#endif /* __GAME_BOARD_TESTS_HPP__ */