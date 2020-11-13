#ifndef __LEVEL_BUTTON_LIST_HPP__
#define __LEVEL_BUTTON_LIST_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Game/ETLevelProgress.hpp"

class ReflectContext;

class LevelButtonItem {
public:

    static void Reflect(ReflectContext& ctx);

public:

    std::string levelId;
    std::string levelName;
    EntityId buttonId;
    EntityId senderId;
    int startsRequired;
};

class LevelButtonList : public EntityLogic,
    public ETNode<ETLevelButtonList> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    LevelButtonList();
    virtual ~LevelButtonList();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETLevelButtonList
    void ET_updateLevelProgress() override;
    const char* ET_getLevelNameForSender(EntityId senderId) const override;
    int ET_getTotalStars() const override;
    int ET_getDoneStars() const override;

private:

    std::vector<LevelButtonItem> levelButtons;
};

#endif /* __LEVEL_BUTTON_LIST_HPP__ */