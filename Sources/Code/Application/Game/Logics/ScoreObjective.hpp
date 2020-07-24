#ifndef __SCORE_OBJECTIVE_HPP__
#define __SCORE_OBJECTIVE_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Game/Logics/ObjectiveTarget.hpp"

class ReflectContext;

class ScoreObjective : public EntityLogic,
    public ETNode<ETGameBoardElemDestoryEvents> {
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

private:

    bool isObjectiveComplete(ObjectiveTarget& target) const;

private:

    ObjectiveType type;
    int currentScore;
    ObjectiveTarget oneStarTarget;
    ObjectiveTarget twoStarsTarget;
    ObjectiveTarget threeStarsTarget;
};

#endif /* __SCORE_OBJECTIVE_HPP__ */