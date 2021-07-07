#ifndef __GAME_MOVES_LIMIT_LOGIC_HPP__
#define __GAME_MOVES_LIMIT_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/ETGame.hpp"

class GameMovesLimitLogic : public EntityLogic,
    public ETNode<ETGameLimits>,
    public ETNode<ETGameStateEvents>,
    public ETNode<ETGameBoardInteractionEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameMovesLimitLogic();
    virtual ~GameMovesLimitLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameLimits
    void ET_setLabelId(EntityId newLabelId) override;
    EGameLimitType ET_getLimitType() const override;

    // ETGameBoardInteractionEvents
    void ET_onElemMoved() override;

    // ETGameStateEvents
    void ET_onGameEnterState(EGameState state) override;
    void ET_onGameLeaveState(EGameState state) override;

private:

    EntityId labelId;
    int movesLimit;
    int currMoves;
};

#endif /* __GAME_MOVES_LIMIT_LOGIC_HPP__ */