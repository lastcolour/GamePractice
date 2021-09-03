#ifndef __ET_GAME_ELEM_HPP__
#define __ET_GAME_ELEM_HPP__

#include "UI/UIProxyContainer.hpp"

enum class EPatternType;

enum class EBoardElemState {
    Static = 0,
    Falling,
    Switching,
    Landing,
    Merging,
    Triggering,
    Destroying,
    Destroyed
};

enum class EBoardElemType {
    None = 0,
    Red,
    Blue,
    Green,
    Purple,
    Yellow,
    HRocket,
    VRocket,
    Bomb,
    Star
};

struct ElemMergeTask {
    std::vector<EntityId> elems;
    EBoardElemType elemType;
};

struct ETGameBoardElem {
    virtual ~ETGameBoardElem() = default;
    virtual void ET_setState(EBoardElemState newState) = 0;
    virtual EBoardElemState ET_getState() const = 0;
    virtual void ET_setType(EBoardElemType newType) = 0 ;
    virtual EBoardElemType ET_getType() const = 0;
    virtual void ET_triggerDestroy(EntityId sourceId) = 0;
    virtual void ET_triggerLand() = 0;
    virtual void ET_setSelected(bool flag) = 0;
    virtual bool ET_canMatch() const = 0;
    virtual bool ET_canSwitch() const = 0;
    virtual void ET_onLandPlayed() = 0;
    virtual void ET_onDestroyDone() = 0;
    virtual void ET_onTriggerDone() = 0;
    virtual void ET_onMergeDone() = 0;
};

struct ETGameBoardRenderElem {
    virtual ~ETGameBoardRenderElem() = default;
    virtual void ET_initRender(UIProxyContainer& rootContainer, const Vec2& elemSize) = 0;
    virtual void ET_deinitRender(UIProxyContainer& rootContainer) = 0;
};

struct ETBoardElemDestroyAnimation {
    virtual ~ETBoardElemDestroyAnimation() = default;
    virtual void ET_playDestroy() = 0;
};

struct ETGameBoardElemSelectAnimation {
    virtual ~ETGameBoardElemSelectAnimation() = default;
    virtual void ET_playSelect() = 0;
    virtual void ET_playDeselect() = 0;
};

struct ETGameBoardElemLandAnimation {
    virtual ~ETGameBoardElemLandAnimation() = default;
    virtual void ET_playLand() = 0;
};

struct ETGameBoardElemMergeManager {
    virtual ~ETGameBoardElemMergeManager() = default;
    virtual void ET_createMergeTask(const ElemMergeTask& newTask) = 0;
    virtual bool ET_hasMergeTasks() const = 0;
    virtual void ET_updateMergeTasks(float dt) = 0;
};

struct ETGameBoardElemTriggerLogic {
    virtual ~ETGameBoardElemTriggerLogic() = default;
    virtual void ET_start() = 0;
    virtual bool ET_update(float dt) = 0;
    virtual void ET_setSwapedElem(EntityId elemId) = 0;
};

struct ETGameBoardElemTriggerManager {
    virtual ~ETGameBoardElemTriggerManager() = default;
    virtual void ET_createTriggerTask(EntityId elemId, bool applyDelay) = 0;
    virtual void ET_updateTriggerTasks(float dt) = 0;
    virtual bool ET_hasTriggerTasks() const = 0;
};

#endif /* __ET_GAME_ELEM_HPP__ */