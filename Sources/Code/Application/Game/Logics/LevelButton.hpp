#ifndef __LEVEL_BUTTON_HPP__
#define __LEVEL_BUTTON_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETLevelProgress.hpp"
#include "Game/ViewScripts/ProgressionStars.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIAnimation.hpp"

class ReflectContext;

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
    void ET_setLevelStars(int count) override;
    EntityId ET_getSenderId() const override;
    void ET_setLevelState(ELevelButtonState newState) override;
    void ET_scheduleChanges(EventSequence& eventSeq, ELevelButtonState newState, int prevStarCount, int newStarCount) override;

private:

    EntityId levelIdLabel;
    EntityId senderId;
    EntityId lockedBackgroundId;
    EntityId unlockedBackgroundId;
    EntityId completedBackgroundId;
    ProgressionStars stars;
    ELevelButtonState state;
};

#endif /* __LEVEL_BUTTON_HPP__ */