#ifndef __PRE_GAME_STATE_HPP__
#define __PRE_GAME_STATE_HPP__

#include "Core/ETPrimitives.hpp"
#include "Game/ETGameInterfaces.hpp"

class PreGameState : public ETNode<ETGameBoardAppearAnimationEvents> {
public:

    PreGameState();
    ~PreGameState();

    void onEnter(EntityId gameEntityId);
    void onLeave();

    // ETGameBoardAppearAnimationEvents
    void ET_onBoardAppeared() override;

private:

};

#endif /* __PRE_GAME_STATE_HPP__ */