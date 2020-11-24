#ifndef __ET_GAME_ELEM_HPP__
#define __ET_GAME_ELEM_HPP__

enum class EBoardElemMoveState {
    Falling,
    Switching,
    Static,
};

enum class EBoardElemLifeState {
    Void = 0,
    Alive,
    Destroying
};

enum class EBoardElemType {
    None = 0,
    Red,
    Blue,
    Green,
    Purple,
    Yellow
};

struct ETGameBoardElem {
    virtual ~ETGameBoardElem() = default;
    virtual void ET_setMoveState(EBoardElemMoveState newState) = 0;
    virtual EBoardElemMoveState ET_getMoveState() const = 0;
    virtual void ET_setLifeState(EBoardElemLifeState newState) = 0;
    virtual EBoardElemLifeState ET_getLifeState() const = 0;
    virtual EBoardElemType ET_getType() const = 0;
    virtual void ET_triggerDestroy() = 0;
    virtual void ET_setSelected(bool flag) = 0;
    virtual bool ET_canMatch() const = 0;
    virtual bool ET_canSwitch() const = 0;
};

struct ETBoardElemDetroyAnimation {
    virtual ~ETBoardElemDetroyAnimation() = default;
    virtual void ET_playDestroy() = 0;
};

struct ETBoardElemDetroyAnimationEvents {
    virtual ~ETBoardElemDetroyAnimationEvents() = default;
    virtual void ET_onDestroyAnimEnded() = 0;
};

struct ETGameBoardElemSelectAnimation {
    virtual ~ETGameBoardElemSelectAnimation() = default;
    virtual void ET_playSelect() = 0;
    virtual void ET_playDeselect() = 0;
};

struct ETGameBoardAnimation {
    virtual ~ETGameBoardAnimation() = default;
    virtual void ET_zoomOut() = 0;
};

#endif /* __ET_GAME_ELEM_HPP__ */