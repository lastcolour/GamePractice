#ifndef __ET_LEVEL_PROGRESS_HPP__
#define __ET_LEVEL_PROGRESS_HPP__

#include "Core/Core.hpp"

class LevelProgress;
struct LevelProgressDelta;
class EventSequence;

enum class ELevelButtonState {
    Locked = 0,
    Unlocked
};

struct ETLevelsProgression {
    virtual ~ETLevelsProgression() = default;
    virtual const LevelProgress* ET_getLevelProgress(const char* levelName) const = 0;
    virtual void ET_setLevelProgress(const LevelProgress& newLevelProgress) = 0;
    virtual int ET_getStarsDone() const = 0;
    virtual const LevelProgressDelta* ET_getProgressDelta() const = 0;
    virtual void ET_resetProgressDelta() = 0;
};

struct ETLevelButton {
    virtual ~ETLevelButton() = default;
    virtual EntityId ET_getSenderId() const = 0;
    virtual void ET_setLevelId(const char* levelId) = 0;
    virtual void ET_setLevelState(ELevelButtonState newState, int newStarsCount) = 0;
    virtual void ET_scheduleChanges(EventSequence& eventSeq, ELevelButtonState newState, int newStarCount) = 0;
};

struct ETLevelButtonList {
    virtual ~ETLevelButtonList() = default;
    virtual void ET_updateLevelProgress(EventSequence& eventSeq) = 0;
    virtual const char* ET_getLevelNameForSender(EntityId senderId) const = 0;
    virtual int ET_getTotalStars() const = 0;
    virtual int ET_getDoneStars() const = 0;
};

#endif /* __ET_LEVEL_PROGRESS_HPP__ */