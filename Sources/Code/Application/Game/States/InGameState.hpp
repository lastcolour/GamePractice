#ifndef __IN_GAME_STATE_HPP__
#define __IN_GAME_STATE_HPP__

#include "Core/Core.hpp"
#include "Game/ETGameInterfaces.hpp"

class InGameState : public ETNode<ETGameEndTimerEvents> {
public:

    InGameState();
    ~InGameState();

    void onEnter();
    void onLeave();

    void connect(EntityId entityId);

    // ETGameEndTimerEvents
    void ET_onGameTimeOut() override;

private:

};

#endif /* __IN_GAME_STATE_HPP__ */