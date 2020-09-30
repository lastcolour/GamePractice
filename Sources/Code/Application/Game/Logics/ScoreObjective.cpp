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
    currentProgress(ObjectiveProgress::Fail) {
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

void ScoreObjective::ET_resetScore() {
    currentScore = 0;
    currentProgress = ObjectiveProgress::Fail;
    ET_SendEvent(&ETGameScoreUpdater::ET_setGameScore, 0);
}

int ScoreObjective::ET_getGameScore() const {
    return currentScore;
}

ObjectiveProgress ScoreObjective::ET_getObjectiveProgress() const {
    return currentProgress;
}

void ScoreObjective::ET_onElemsDestroyed(EntityId elemId) {
    currentScore += 1;
    ET_SendEvent(&ETGameScoreUpdater::ET_setGameScore, currentScore);
    switch(currentProgress) {
        case ObjectiveProgress::Fail: {
            if(isObjectiveComplete(oneStarTarget)) {
                currentProgress = ObjectiveProgress::OneStar;
                LogDebug("[ScoreObjective::ET_onElemsDestroyed] One-star objective done");
                ET_SendEvent(&ETGameObjectiveEvents::ET_onObjectiveCompleted, ObjectiveProgress::OneStar);
            } else {
                break;
            }
            [[fallthrough]];
        }
        case ObjectiveProgress::OneStar: {
            if(isObjectiveComplete(twoStarsTarget)) {
                currentProgress = ObjectiveProgress::TwoStars;
                LogDebug("[ScoreObjective::ET_onElemsDestroyed] Two-stars objective done");
                ET_SendEvent(&ETGameObjectiveEvents::ET_onObjectiveCompleted, ObjectiveProgress::TwoStars);
            } else {
                break;
            }
            [[fallthrough]];
        }
        case ObjectiveProgress::TwoStars: {
            currentProgress = ObjectiveProgress::ThreeStars;
            if(isObjectiveComplete(threeStarsTarget)) {
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