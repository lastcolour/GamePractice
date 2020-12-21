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

ScoreObjective::ScoreObjective() {
}

ScoreObjective::~ScoreObjective() {
}

void ScoreObjective::init() {
    ETNode<ETGameBoardElemDestoryEvents>::connect(getEntityId());
    ETNode<ETGameScore>::connect(getEntityId());
}

void ScoreObjective::deinit() {
}

bool ScoreObjective::isObjectiveComplete(ObjectiveTarget& target) const {
    if(currentResult.score >= target.minScore) {
        return true;
    }
    return false;
}

const EndGameResult& ScoreObjective::ET_getGameResult() const {
    return currentResult;
}

void ScoreObjective::ET_setGameTime(float gameTime) {
    currentResult.gameTime = gameTime;
}

void ScoreObjective::ET_onElemsDestroyed(EntityId elemId) {
    currentResult.score += 1;
    ET_SendEvent(&ETGameScoreUpdater::ET_setGameScore, currentResult.score);
    switch(currentResult.objectiveCompleted) {
        case ObjectiveProgress::Fail: {
            if(isObjectiveComplete(oneStarTarget)) {
                currentResult.objectiveCompleted = ObjectiveProgress::OneStar;
                LogDebug("[ScoreObjective::ET_onElemsDestroyed] One-star objective done");
                ET_SendEvent(&ETGameObjectiveEvents::ET_onObjectiveCompleted, ObjectiveProgress::OneStar);
            } else {
                break;
            }
            [[fallthrough]];
        }
        case ObjectiveProgress::OneStar: {
            if(isObjectiveComplete(twoStarsTarget)) {
                currentResult.objectiveCompleted = ObjectiveProgress::TwoStars;
                LogDebug("[ScoreObjective::ET_onElemsDestroyed] Two-stars objective done");
                ET_SendEvent(&ETGameObjectiveEvents::ET_onObjectiveCompleted, ObjectiveProgress::TwoStars);
            } else {
                break;
            }
            [[fallthrough]];
        }
        case ObjectiveProgress::TwoStars: {
            if(isObjectiveComplete(threeStarsTarget)) {
                currentResult.objectiveCompleted = ObjectiveProgress::ThreeStars;
                LogDebug("[ScoreObjective::ET_onElemsDestroyed] Three-stars objective done");
                ET_SendEvent(&ETGameObjectiveEvents::ET_onObjectiveCompleted, ObjectiveProgress::ThreeStars);
            }
            break;
        }
        case ObjectiveProgress::ThreeStars: {
            break;
        }
        default: {
            assert(false && "Invalid objective state");
        }
    }
}