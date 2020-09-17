#ifndef __SCORE_OBJECTIVE_HPP__
#define __SCORE_OBJECTIVE_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Game/ETGameInterfaces.hpp"
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
    void ET_resetScore() override;
    int ET_getGameScore() const override;
    ObjectiveProgress ET_getObjectiveProgress() const override;

private:

    bool isObjectiveComplete(ObjectiveTarget& target) const;

private:

    ObjectiveProgress currentProgress;
    int currentScore;
    ObjectiveTarget oneStarTarget;
    ObjectiveTarget twoStarsTarget;
    ObjectiveTarget threeStarsTarget;
};

#endif /* __SCORE_OBJECTIVE_HPP__ */