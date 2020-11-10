#ifndef __IN_GAME_STATE_HPP__
#define __IN_GAME_STATE_HPP__

#include "Core/Core.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGame.hpp"
#include "Core/ETPrimitives.hpp"
#include "Game/ETGameScore.hpp"
#include "Game/ETGameTimer.hpp"

class InGameState : public ETNode<ETGameEndTimerEvents>,
    public ETNode<ETGameObjectiveEvents>,
    public ETNode<ETGameTimerEvents> {
public:

    InGameState();
    ~InGameState();

    void onEnter(EntityId gameEntityId);
    void onLeave();

    // ETGameEndTimerEvents
    void ET_onGameTimeOut() override;

    // ETGameObjectiveEvents
    void ET_onObjectiveCompleted(ObjectiveProgress type) override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

private:

    float gameTime;
};

#endif /* __IN_GAME_STATE_HPP__ */