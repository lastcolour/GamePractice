#include "GameStateTests.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "TestUtils/GameTestUtils.hpp"
#include "ETApplicationInterfaces.hpp"
#include "UI/ETUIInterfaces.hpp"

namespace {

const char* GAME_VIEW = "Game/GameBoard.json";

}; // namespace

void GameStateTests::SetUpTestCase() {
    ConsoleAppTests::SetUpTestCase();
    ET_SendEvent(&ETUIViewStack::ET_pushView, GAME_VIEW);
}

void GameStateTests::SetUp() {
    ET_SendEvent(&ETGameStateManager::ET_initGame);
    ET_SendEvent(&ETGameStateManager::ET_startGame);
}

void GameStateTests::TearDown() {
    ET_SendEvent(&ETGameStateManager::ET_interruptGame);
}

bool GameStateTests::isEndTimerTicking() const {
    float startRemainingTime = 0.f;
    ET_SendEventReturn(startRemainingTime, &ETGameEndTimer::ET_getRemainingTime);

    float tickDuration = 0.5f;
    ET_SendEvent(&ETTimerEvents::ET_onTick, tickDuration);

    float endRemainingTime = 0.f;
    ET_SendEventReturn(endRemainingTime, &ETGameEndTimer::ET_getRemainingTime);

    return startRemainingTime > endRemainingTime + 0.01f;
}

bool GameStateTests::isInteractionEnabled() const {
    bool canInteract = true;
    ET_SendEventReturn(canInteract, &ETGAmeBoardInteractionLogic::ET_canInteract);
    return canInteract;
}

TEST_F(GameStateTests, CheckPreGameState) {
    EXPECT_FALSE(isEndTimerTicking());
    EXPECT_FALSE(isInteractionEnabled());
}

TEST_F(GameStateTests, CheckPreGame_Resize) {
    // Finish board appearing
}

TEST_F(GameStateTests, CheckPreGame_Pause) {
    // Finish board appearing
}

TEST_F(GameStateTests, CheckInGameState) {
    WaitPreGameEnd();

    EXPECT_TRUE(isEndTimerTicking());
    EXPECT_TRUE(isInteractionEnabled());
}

TEST_F(GameStateTests, CheckPostGameState) {
    WaitPreGameEnd();
    WaitInGameEnd();

    EXPECT_FALSE(isEndTimerTicking());
    EXPECT_FALSE(isInteractionEnabled());
}