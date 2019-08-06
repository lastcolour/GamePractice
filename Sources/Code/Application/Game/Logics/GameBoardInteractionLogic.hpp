#ifndef __GAME_BOARD_INTERACTION_LOGIC_HPP__
#define __GAME_BOARD_INTERACTION_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "ETApplicationInterfaces.hpp"

class GameBoardInteractionLogic : public GameLogic,
    public ETNode<ETInputEvents> {
public:

    GameBoardInteractionLogic();
    virtual ~GameBoardInteractionLogic();

    // GameLogic
    bool init() override;
    bool serialize(const JSONNode& node) override;

    // ETInputEvents
    void ET_onTouch(ETouchType touchType, const Vec2i& pt) override;

private:

    void onStartElemMove();
    void onElemMove();
    void onEndElemMove(const Vec2i& pt);

private:

    Vec2i startPt;
    EntityId activeElemId;
};

#endif /* __GAME_BOARD_INTERACTION_LOGIC_HPP__ */