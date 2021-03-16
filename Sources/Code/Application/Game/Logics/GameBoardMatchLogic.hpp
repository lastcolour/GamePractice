#ifndef __GAME_BOARD_MATCH_LOGIC_HPP__
#define __GAME_BOARD_MATCH_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/ETGameElem.hpp"

#include <unordered_set>

class GameBoardMatchLogic : public EntityLogic,
    public ETNode<ETGameBoardMatcher> {

    using MatchElemCollectionT = std::unordered_set<EntityId>;

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

    bool isElementsMatch(EntityId firstId, EntityId secondId) const;
    bool findMatchLine(const Vec2i& startPt, int lineLen, bool isHorizontal, MatchElemCollectionT& result);
    void triggerDestroyEffect(EntityId elemId, const Transform& emiTm);

private:

    EntityId redDestroyEffectId;
    EntityId blueDestroyEffectId;
    EntityId yellowDestroyEffectId;
    EntityId greenDestroyEffectId;
    EntityId purpleDestroyEffectId;
    float destroyEffectScale;
    int minLineLen;
    int maxLineLen;
};

#endif /* __GAME_BOARD_MATCH_LOGIC_HPP__ */