#ifndef __POST_GAME_STATE_HPP__
#define __POST_GAME_STATE_HPP__

#include "Core/ETPrimitives.hpp"
#include "Game/ETGameInterfaces.hpp"

class PostGameState : public ETNode<ETGameEndResult> {
public:

    PostGameState();
    ~PostGameState();

    void onEnter();
    void onLeave();

    void connect(EntityId entityId);

    // ETGameEndResult
    const EndGameResult* ET_getGameEndResult() const override;

private:

    void setupEndResult();

private:

    std::unique_ptr<EndGameResult> endResult;
};

#endif /* __POST_GAME_STATE_HPP__ */