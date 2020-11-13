#ifndef __ET_LEVEL_PROGRESS_HPP__
#define __ET_LEVEL_PROGRESS_HPP__

#include "Core/Core.hpp"

class LevelProgress;

enum class ELevelButtonState {
    Locked = 0,
    Unlocked,
    Completed
};

struct ETLevelsProgression {
    virtual ~ETLevelsProgression() = default;
    virtual const LevelProgress* ET_getLevelProgress(const char* levelName) const = 0;
    virtual void ET_setLevelProgress(const LevelProgress& newLevelProgress) = 0;
    virtual int ET_getStarsDone() const = 0;
};

struct ETLevelButton {
    virtual ~ETLevelButton() = default;
    virtual void ET_setLevelId(const char* levelId) = 0;
    virtual void ET_setLevelState(ELevelButtonState newState) = 0;
    virtual void ET_setLevelStars(int count) = 0;
    virtual EntityId ET_getSenderId() const = 0;
};

struct ETLevelButtonList {
    virtual ~ETLevelButtonList() = default;
    virtual void ET_updateLevelProgress() = 0;
    virtual const char* ET_getLevelNameForSender(EntityId senderId) const = 0;
    virtual int ET_getTotalStars() const = 0;
    virtual int ET_getDoneStars() const = 0;
};

#endif /* __ET_LEVEL_PROGRESS_HPP__ */