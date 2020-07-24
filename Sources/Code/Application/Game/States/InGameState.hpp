#ifndef __IN_GAME_STATE_HPP__
#define __IN_GAME_STATE_HPP__

#include "Core/Core.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGame.hpp"
#include "Core/ETPrimitives.hpp"

class InGameState : public ETNode<ETGameEndTimerEvents>,
    public ETNode<ETGameObjectiveEvents> {
public:

    InGameState();
    ~InGameState();

    void onEnter(EntityId gameEntityId);
    void onLeave();

    // ETGameEndTimerEvents
    void ET_onGameTimeOut() override;

    // ETGameObjectiveEvents
    void ET_onObjectiveCompleted(ObjectiveType type) override;

private:

};

#endif /* __IN_GAME_STATE_HPP__ */