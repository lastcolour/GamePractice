#ifndef __POST_GAME_STATE_HPP__
#define __POST_GAME_STATE_HPP__

#include "Core/ETPrimitives.hpp"
#include "Game/ETGameInterfaces.hpp"

class PostGameState : public ETNode<ETGameEndResult>,
    public ETNode<ETGameTimerEvents> {
public:

    PostGameState();
    ~PostGameState();

    void onEnter(EntityId gameEntityId);
    void onLeave();

    // ETGameEndResult
    const EndGameResult* ET_getGameEndResult() const override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

private:

    void setupEndResult();

private:

    std::unique_ptr<EndGameResult> endResult;
    float postGameTime;
    EntityId gameEntityId;
};

#endif /* __POST_GAME_STATE_HPP__ */