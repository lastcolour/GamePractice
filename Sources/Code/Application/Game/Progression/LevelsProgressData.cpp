#include "Game/Progression/LevelsProgressData.hpp"

LevelProgress::LevelProgress() :
    completeTime(-1.f),
    score(0),
    stars(0) {
}

void LevelProgress::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<LevelProgress>("LevelProgress")) {
        classInfo->addField("score", &LevelProgress::score);
        classInfo->addField("stars", &LevelProgress::stars);
        classInfo->addField("completeTime", &LevelProgress::completeTime);
        classInfo->addField("name", &LevelProgress::name);
    }
}

void LevelProgressList::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<LevelProgressList>("LevelProgressList")) {
        classInfo->addField("levels", &LevelProgressList::levels);
    }
}