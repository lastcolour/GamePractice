#ifndef __POST_GAME_STATE_HPP__
#define __POST_GAME_STATE_HPP__

#include "Core/ETPrimitives.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/ETGameTimer.hpp"
#include "Game/ETGameScore.hpp"

#include <memory>

class PostGameState : public ETNode<ETGameEndResult>,
    public ETNode<ETGameTimerEvents>,
    public ETNode<ETGameBoardEvents> {
public:

    PostGameState();
    ~PostGameState();

    void onEnter(EntityId gameEntityId);
    void onLeave();

    // ETGameEndResult
    const EndGameResult& ET_getLastGameResult() const override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

    // ETGameBoardEvents
    void ET_onAllElemsStatic() override;

private:

    void setupEndResult();

private:

    EndGameResult endResult;
    EntityId gameEntityId;
    float postGameTime;
};

#endif /* __POST_GAME_STATE_HPP__ */