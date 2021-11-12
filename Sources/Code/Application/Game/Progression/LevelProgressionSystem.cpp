#include "Game/Progression/LevelProgressionSystem.hpp"
#include "Core/ETAssets.hpp"

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
        if(!Reflect::LoadObjectFromLocalFile(levelsData, LEVEL_PROGRESS_FILE)) {
            LogWarning("[LevelProgressionSystem::init] Can't load saved progress: '%s'", LEVEL_PROGRESS_FILE);
            isProgressExist = false;
        }
    }
    if(!isProgressExist) {
        if(!Reflect::SaveObjectToLocalFile(levelsData, LEVEL_PROGRESS_FILE)) {
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

    bool newProgHasMoreStars = false;
    if(progressDelta->prev.stars <= newLevelProgress.stars) {
        newProgHasMoreStars = true;
    }
    bool newProgIsBetter = false;
    if(newProgHasMoreStars) {
        if(progressDelta->prev.completeTime > newLevelProgress.completeTime) {
            newProgIsBetter = true;
        }
        if(progressDelta->prev.score < newLevelProgress.score) {
            newProgIsBetter = true;
        }
    }

    if(!newProgIsBetter) {
        return;
    }

    Reflect::AsyncSaveObjectToLocalFile(levelsData, LEVEL_PROGRESS_FILE);
}

const LevelProgressDelta* LevelProgressionSystem::ET_getProgressDelta() const {
    return progressDelta.get();
}

void LevelProgressionSystem::ET_resetProgressDelta() {
    progressDelta.reset();
}