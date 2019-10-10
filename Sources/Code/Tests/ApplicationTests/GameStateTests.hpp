#ifndef __GAME_STATE_TESTS_HPP__
#define __GAME_STATE_TESTS_HPP__

#include "TestUtils/ConsoleAppTests.hpp"

class GameStateTests : public ConsoleAppTests {
protected:

    static void SetUpTestCase();

protected:

    void SetUp() override;
    void TearDown() override;

protected:

    void startGame();

    bool isEndTimerTicking() const;
    bool isInteractionEnabled() const;
};

#endif /* __GAME_STATE_TESTS_HPP__ */