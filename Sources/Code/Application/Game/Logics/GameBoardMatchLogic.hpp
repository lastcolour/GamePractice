#ifndef __GAME_BOARD_MATCH_LOGIC_HPP__
#define __GAME_BOARD_MATCH_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/ETGameElem.hpp"
#include "UI/UIProxyContainer.hpp"
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
    void ET_destoryMatchedElems() override;

private:

    void triggerDestroyEffect(EntityId elemId, const Transform& emiTm);

private:

    BoardMatchState boardMatchState;
    UIProxyContainer uiProxies;
    EntityId redDestroyEffectId;
    EntityId blueDestroyEffectId;
    EntityId yellowDestroyEffectId;
    EntityId greenDestroyEffectId;
    EntityId purpleDestroyEffectId;
    float destroyEffectScale;
};

#endif /* __GAME_BOARD_MATCH_LOGIC_HPP__ */