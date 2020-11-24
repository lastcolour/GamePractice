#ifndef __SCORE_OBJECTIVE_HPP__
#define __SCORE_OBJECTIVE_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/Logics/ObjectiveTarget.hpp"
#include "Game/ETGameScore.hpp"

class ReflectContext;

class ScoreObjective : public EntityLogic,
    public ETNode<ETGameBoardElemDestoryEvents>,
    public ETNode<ETGameScore> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ScoreObjective();
    virtual ~ScoreObjective();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETGameBoardElemDestoryEvents
    void ET_onElemsDestroyed(EntityId elemId) override;

    // ETGameScore
    void ET_setGameTime(float gameTime) override;
    const EndGameResult& ET_getGameResult() const override;

private:

    bool isObjectiveComplete(ObjectiveTarget& target) const;

private:

    EndGameResult currentResult;
    ObjectiveTarget oneStarTarget;
    ObjectiveTarget twoStarsTarget;
    ObjectiveTarget threeStarsTarget;
};

#endif /* __SCORE_OBJECTIVE_HPP__ */