#include "Game/Logics/ScoreObjective.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>

void ScoreObjective::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ScoreObjective>("ScoreObjective")) {
        classInfo->addField("oneStar", &ScoreObjective::oneStarTarget);
        classInfo->addField("twoStars", &ScoreObjective::twoStarsTarget);
        classInfo->addField("threeStars", &ScoreObjective::threeStarsTarget);
    }
}

ScoreObjective::ScoreObjective() :
    currentScore(0),
    type(ObjectiveType::Fail) {
}

ScoreObjective::~ScoreObjective() {
}

bool ScoreObjective::init() {
    ETNode<ETGameBoardElemDestoryEvents>::connect(getEntityId());
    return true;
}

void ScoreObjective::deinit() {
}

bool ScoreObjective::isObjectiveComplete(ObjectiveTarget& target) const {
    if (currentScore >= target.minScore) {
        return true;
    }
    return false;
}

void ScoreObjective::ET_onElemsDestroyed(EntityId elemId) {
    currentScore += 1;
    switch(type) {
        case ObjectiveType::Fail: {
            if(isObjectiveComplete(oneStarTarget)) {
                type = ObjectiveType::OneStar;
                LogDebug("[ScoreObjective::ET_onElemsDestroyed] One-star objective done");
                ET_SendEvent(&ETGameObjectiveEvents::ET_onObjectiveCompleted, ObjectiveType::OneStar);
            } else {
                break;
            }
            [[fallthrough]];
        }
        case ObjectiveType::OneStar: {
            if(isObjectiveComplete(twoStarsTarget)) {
                type = ObjectiveType::TwoStars;
                LogDebug("[ScoreObjective::ET_onElemsDestroyed] Two-stars objective done");
                ET_SendEvent(&ETGameObjectiveEvents::ET_onObjectiveCompleted, ObjectiveType::TwoStars);
            } else {
                break;
            }
            [[fallthrough]];
        }
        case ObjectiveType::TwoStars: {
            type = ObjectiveType::ThreeStars;
            if(isObjectiveComplete(threeStarsTarget)) {
                LogDebug("[ScoreObjective::ET_onElemsDestroyed] Three-stars objective done");
                ET_SendEvent(&ETGameObjectiveEvents::ET_onObjectiveCompleted, ObjectiveType::ThreeStars);
            }
            break;
        }
        case ObjectiveType::ThreeStars: {
            break;
        }
        default: {
            assert(false && "Invalid objective state");
        }
    }
}