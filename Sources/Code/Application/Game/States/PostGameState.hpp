#ifndef __POST_GAME_STATE_HPP__
#define __POST_GAME_STATE_HPP__

#include "Core/ETPrimitives.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/ETGameScore.hpp"
#include "Game/ETGameElem.hpp"

#include <memory>

class PostGameState : public ETNode<ETGameEndResult>,
    public ETNode<ETGameBoardEvents>,
    public ETNode<ETGameBoardAnimationEvents> {
public:

    PostGameState();
    ~PostGameState();

    void onEnter(EntityId gameEntityId);
    void onLeave();

    // ETGameEndResult
    const EndGameResult& ET_getLastGameResult() const override;

    // ETGameBoardEvents
    void ET_onAllElemsStatic() override;

    // ETGameBoardAnimationEvents
    void ET_onZoomOutPlayed() override;

private:

    void tryEndPostGame();
    void setupEndResult();

private:

    EndGameResult endResult;
    EntityId gameEntityId;
    bool allElemStatic;
    bool zoomOutPlayed;
};

#endif /* __POST_GAME_STATE_HPP__ */