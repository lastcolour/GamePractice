#ifndef __IN_GAME_STATE_HPP__
#define __IN_GAME_STATE_HPP__

#include "Core/Core.hpp"

class InGameState {
public:

    InGameState();
    ~InGameState();

    void onEnter();
    void onLeave();

    void connect(EntityId entityId);

private:

};

#endif /* __IN_GAME_STATE_HPP__ */