#ifndef __ET_GAME_ELEM_HPP__
#define __ET_GAME_ELEM_HPP__

enum class EBoardElemState {
    Static = 0,
    Falling,
    Switching,
    Landing,
    Destroying,
    Destroyed
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
    virtual void ET_setElemState(EBoardElemState newState) = 0;
    virtual EBoardElemState ET_getState() const = 0;
    virtual EBoardElemType ET_getType() const = 0;
    virtual void ET_triggerDestroy() = 0;
    virtual void ET_triggerLand() = 0;
    virtual void ET_setSelected(bool flag) = 0;
    virtual bool ET_canMatch() const = 0;
    virtual bool ET_canSwitch() const = 0;
    virtual void ET_onLandPlayed() = 0;
    virtual void ET_onDestroyPlayed() = 0;
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

#endif /* __ET_GAME_ELEM_HPP__ */