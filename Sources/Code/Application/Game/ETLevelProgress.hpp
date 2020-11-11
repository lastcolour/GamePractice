#ifndef __ET_LEVEL_PROGRESS_HPP__
#define __ET_LEVEL_PROGRESS_HPP__

struct ETLevelsProgression {
    virtual ~ETLevelsProgression() = default; 
};

struct ETLevelButton {
    virtual ~ETLevelButton() = default;
    virtual void ET_setLevelId(const char* levelId) = 0;
    virtual void ET_setLevelLocked(bool flag) = 0;
    virtual void ET_setLevelStars(int count) = 0;
    virtual EntityId ET_getSenderId() const = 0;
};

struct ETLevelButtonList {
    virtual ~ETLevelButtonList() = default;
    virtual const char* ET_getLevelNameForSender(EntityId senderId) const = 0;
};

#endif /* __ET_LEVEL_PROGRESS_HPP__ */