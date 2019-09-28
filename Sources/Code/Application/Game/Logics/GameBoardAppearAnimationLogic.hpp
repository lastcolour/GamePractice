#ifndef __GAME_BOARD_APPEAR_ANIMATION_LOGIC_HPP__
#define __GAME_BOARD_APPEAR_ANIMATION_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameInterfaces.hpp"

class GameBoardAppearAnimationLogic : public EntityLogic,
    public ETNode<ETGameTimerEvents>,
    public ETNode<ETGameBoardAppearAnimation> {
public:

    GameBoardAppearAnimationLogic();
    virtual ~GameBoardAppearAnimationLogic();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

    // ETGameBoardAppearAnimation
    void ET_startBoardAppearing() override;

private:

    void moveAllElemsOnTop();
    void moveCollumnElemDown(int collumnIdx);

private:

    float duration;
    float currDuration;
    Vec2i boardSize;
};

#endif /* __GAME_BOARD_APPEAR_ANIMATION_LOGIC_HPP__ */