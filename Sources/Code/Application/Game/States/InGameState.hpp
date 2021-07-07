#ifndef __IN_GAME_STATE_HPP__
#define __IN_GAME_STATE_HPP__

#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGame.hpp"
#include "Game/ETGameScore.hpp"
#include "Game/ETGameTimer.hpp"
#include "Game/ETGameBoard.hpp"

class InGameState : public ETNode<ETGameObjectiveEvents>,
    public ETNode<ETGameLimitsEvents>,
    public ETNode<ETGameTimerEvents>,
    public ETNode<ETGameBoardEvents> {
public:

    InGameState();
    ~InGameState();

    void onEnter(EntityId gameEntityId);
    void onLeave();

    // ETGameLimitsEvents
    void ET_onGameLimitReached() override;

    // ETGameObjectiveEvents
    void ET_onObjectiveCompleted(ObjectiveProgress type) override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

    // ETGameBoardEvents
    void ET_onAllElemsStatic() override;

private:

    EntityId gameEntityId;
    float gameTime;
};

#endif /* __IN_GAME_STATE_HPP__ */