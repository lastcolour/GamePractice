#ifndef __BOARD_ELEM_SELECT_ANIMATION_HPP__
#define __BOARD_ELEM_SELECT_ANIMATION_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Core/ETPrimitives.hpp"
#include "Game/ETGameElem.hpp"
#include "Math/Vector.hpp"
#include "Game/ETGame.hpp"

class ReflectContext;

class BoardElemSelectAnimation : public EntityLogic, 
    public ETNode<ETGameTimerEvents>,
    public ETNode<ETGameBoardElemSelectAnimation> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    BoardElemSelectAnimation();
    virtual ~BoardElemSelectAnimation();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

    // ETGameBoardElemSelectAnimation
    void ET_playSelect() override;
    void ET_playDeselect() override;

private:

    void updateBackground(float prog);
    void updateForeground(float prog);

private:

    enum class State {
        None = 0,
        Select,
        Deselect
    };

private:

    float currDuration;
    float duration;
    float maxBackgroundScale;
    float minForegroundScale;
    State state;
    Vec2i startSize;
    EntityId backgroundId;
};

#endif /* __BOARD_ELEM_SELECT_ANIMATION_HPP__ */