#ifndef __GAME_BOARD_MATCH_LOGIC_HPP__
#define __GAME_BOARD_MATCH_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/ETGameElem.hpp"
#include "Game/Logics/MatchAlgorithm.hpp"

class GameBoardMatchLogic : public EntityLogic,
    public ETNode<ETGameBoardMatcher> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardMatchLogic();
    virtual ~GameBoardMatchLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoardMatcher
    bool ET_matchElements() override;
    void ET_playDestroyEffect(EntityId elemId) override;

private:

    void matchPattern(const PatternMatch& p);

private:

    BoardMatchState boardMatchState;
    EntityId redDestroyEffectId;
    EntityId blueDestroyEffectId;
    EntityId yellowDestroyEffectId;
    EntityId greenDestroyEffectId;
    EntityId purpleDestroyEffectId;
    float destroyEffectScale;
};

#endif /* __GAME_BOARD_MATCH_LOGIC_HPP__ */