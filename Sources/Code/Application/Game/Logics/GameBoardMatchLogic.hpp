#ifndef __GAME_BOARD_MATCH_LOGIC_HPP__
#define __GAME_BOARD_MATCH_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/Logics/MatchAlgorithm.hpp"
#include "Game/ETGameBoardSpawner.hpp"
#include "Game/Logics/BronKerbosch.hpp"

class GameBoardMatchLogic : public EntityLogic,
    public ETNode<ETGameBoardMatcher>,
    public ETNode<ETGameBoardSpawnerEvents> {
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
    void ET_setLastSwappedElem(EntityId elemId) override;

    // ETGameBoardSpawnerEvents
    void ET_onStartLoading() override;
    void ET_onStartDestroying() override;

private:

    void matchPattern(const PatternMatch& p);

private:

    BronKerboschRequest cacheRequest;
    EntityId lastSwappedElem;
    BoardMatchState boardMatchState;
};

#endif /* __GAME_BOARD_MATCH_LOGIC_HPP__ */