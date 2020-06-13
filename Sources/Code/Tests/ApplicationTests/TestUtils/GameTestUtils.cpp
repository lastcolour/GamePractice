#include "TestUtils/GameTestUtils.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Core/ETTimer.hpp"

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
    float elapsedT = 0.f;
    while (!listener.isBoardAppeard) {
        float dt = 0.01f;
        ET_SendEvent(&ETTimerEvents::ET_onTick, dt);
        elapsedT += dt;
        if(elapsedT > 360.f) {
            break;
        }
    }
}

void WaitInGameEnd() {
    float gameTime = 0.f;
    ET_SendEventReturn(gameTime, &ETGameEndTimer::ET_getRemainingTime);
    gameTime += 0.0001f;
    ET_SendEvent(&ETGameTimerEvents::ET_onGameTick, gameTime);
}