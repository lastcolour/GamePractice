#include "Game/Progression/LevelProgressionSystem.hpp"
#include "Core/ETAssets.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/ETLogger.hpp"
#include "Reflect/ReflectUtils.hpp"

namespace {

const char* LEVEL_PROGRESS_FILE = "LevelsProgress.json";

} // namespace

LevelProgressionSystem::LevelProgressionSystem() {
}

LevelProgressionSystem::~LevelProgressionSystem() {
}

bool LevelProgressionSystem::init() {
    bool isProgressExist = false;
    ET_SendEventReturn(isProgressExist, &ETAssets::ET_isLocalFileExists, LEVEL_PROGRESS_FILE);

    if(isProgressExist) {
        if(!ReflectUtils::LoadObjectFromLocalFile(levelsData, LEVEL_PROGRESS_FILE)) {
            LogWarning("[LevelProgressionSystem::init] Can't load saved progress: '%s'", LEVEL_PROGRESS_FILE);
            isProgressExist = false;
        }
    }
    if(!isProgressExist) {
        if(!ReflectUtils::SaveObjectToLocalFile(levelsData, LEVEL_PROGRESS_FILE)) {
            LogError("[LevelProgressionSystem::init] Can't create levels progress file: '%s'", LEVEL_PROGRESS_FILE);
            return false;
        }
    }
    ETNode<ETLevelsProgression>::connect(getEntityId());
    return true;
}

void LevelProgressionSystem::deinit() {
}

const LevelProgress* LevelProgressionSystem::ET_getLevelProgress(const char* levelName) const {
    for(auto& lvlProgress : levelsData.levels) {
        if(lvlProgress.name == levelName) {
            return &lvlProgress;
        }
    }
    return nullptr;
}

void LevelProgressionSystem::ET_setLevelProgress(const LevelProgress& newLevelProgress) {
    LevelProgress* prevLvlProgress = nullptr;
    for(auto& lvlProgress : levelsData.levels) {
        if(lvlProgress.name == newLevelProgress.name) {
            prevLvlProgress = &lvlProgress;
        }
    }

    progressDelta.reset(new LevelProgressDelta);
    progressDelta->current = newLevelProgress;

    if(prevLvlProgress) {
        progressDelta->prev = *prevLvlProgress;
        *prevLvlProgress = newLevelProgress;
    }  else {
        levelsData.levels.push_back(newLevelProgress);
    }

    bool newProgressIsBetter = false;
    if(progressDelta->prev.completeTime > newLevelProgress.completeTime) {
        newProgressIsBetter = true;
    }
    if(progressDelta->prev.stars < newLevelProgress.stars) {
        newProgressIsBetter = true;
    }
    if(progressDelta->prev.stars < newLevelProgress.score) {
        newProgressIsBetter = true;
    }
    if(!newProgressIsBetter) {
        return;
    }

    if(!ReflectUtils::SaveObjectToLocalFile(levelsData, LEVEL_PROGRESS_FILE)) {
        LogError("[LevelProgressionSystem::ET_setLevelProgress] Can't update level progress file: '%s'", LEVEL_PROGRESS_FILE);
    }
}

int LevelProgressionSystem::ET_getStarsDone() const {
    int starsDone = 0;
    for(auto& lvlProgress : levelsData.levels) {
        starsDone += lvlProgress.stars;
    }
    return starsDone;
}

const LevelProgressDelta* LevelProgressionSystem::ET_getProgressDelta() const {
    return progressDelta.get();
}

void LevelProgressionSystem::ET_resetProgressDelta() {
    progressDelta.reset();
}