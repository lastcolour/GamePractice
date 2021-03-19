#ifndef __LEVEL_BUTTON_HPP__
#define __LEVEL_BUTTON_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETLevelProgress.hpp"
#include "Game/ViewScripts/ProgressionStars.hpp"
#include "UI/ETUIAnimation.hpp"
#include "Audio/SoundEvent.hpp"

struct UnlockedLevelButtonData {
public:

    static void Reflect(ReflectContext& ctx);

public:

    EntityId labelId;
    EntityId buttonId;
    EntityId unlockedRenderId;
    EntityId completedRenderId;
    ProgressionStars stars;
};

struct LockedLevelButtonData {
public:

    static void Reflect(ReflectContext& ctx);

public:

    EntityId labelId;
    EntityId buttonId;
};

class LevelButton : public EntityLogic,
    public ETNode<ETLevelButton> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    LevelButton();
    virtual ~LevelButton();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETLevelButton
    void ET_setLevelId(const char* levelId) override;
    EntityId ET_getSenderId() const override;
    void ET_setLevelState(ELevelButtonState newState, int newStarsCount) override;
    void ET_scheduleChanges(EventSequence& eventSeq, ELevelButtonState newState, int newStarCount) override;
    ELevelButtonState ET_getState() const override;

private:

    UnlockedLevelButtonData unlockedBt;
    LockedLevelButtonData lockedBt;
    SoundEvent unlockSound;
    ELevelButtonState state;
    int starsCount;
};

#endif /* __LEVEL_BUTTON_HPP__ */