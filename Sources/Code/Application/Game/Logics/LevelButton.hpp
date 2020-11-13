#ifndef __LEVEL_BUTTON_HPP__
#define __LEVEL_BUTTON_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETLevelProgress.hpp"
#include "Game/ViewScripts/ProgressionStars.hpp"
#include "Core/ETPrimitives.hpp"

class ReflectContext;

class LevelButton : public EntityLogic,
    public ETNode<ETLevelButton> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    LevelButton();
    virtual ~LevelButton();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETLevelButton
    void ET_setLevelId(const char* levelId) override;
    void ET_setLevelState(ELevelButtonState newState) override;
    void ET_setLevelStars(int count) override;
    EntityId ET_getSenderId() const override;

private:

    EntityId levelIdLabel;
    EntityId senderId;
    EntityId lockedBackgroundId;
    EntityId unlockedBackgroundId;
    EntityId completedBackgroundId;
    ProgressionStars stars;
};

#endif /* __LEVEL_BUTTON_HPP__ */