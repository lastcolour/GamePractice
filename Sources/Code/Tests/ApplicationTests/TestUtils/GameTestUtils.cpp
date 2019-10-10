#include "TestUtils/GameTestUtils.hpp"
#include "Game/ETGameInterfaces.hpp"

namespace {

class GameBoardAppearListener : public ETNode<ETGameBoardAppearAnimationEvents> {
public:

    GameBoardAppearListener() : isBoardAppeard(false) {
        ETNode<ETGameBoardAppearAnimationEvents>::connect(GetETSystem()->createNewEntityId());
    }
    virtual ~GameBoardAppearListener() {}

    // ETGameBoardAppearAnimationEvents
    void ET_onBoardAppeared() override { isBoardAppeard = true; }

public:

    bool isBoardAppeard;
};

} // namespace

void WaitPreGameEnd() {
    GameBoardAppearListener listener;
    while (!listener.isBoardAppeard) {
        float dt = 0.01f;
        ET_SendEvent(&ETGameTimerEvents::ET_onGameTick, dt);
    }
}

void WaitInGameEnd() {
    float gameTime = 0.f;
    ET_SendEventReturn(gameTime, &ETGameEndTimer::ET_getRemainingTime);
    gameTime += 0.0001f;
    ET_SendEvent(&ETGameTimerEvents::ET_onGameTick, gameTime);
}