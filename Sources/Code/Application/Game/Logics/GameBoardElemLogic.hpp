#ifndef __GAME_BOARD_ELEM_LOGIC_HPP__
#define __GAME_BOARD_ELEM_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "UI/UIETInterfaces.hpp"

class GameBoardElemLogic : public GameLogic,
    public ETNode<ETGameBoardElemLogic>,
    public ETNode<ETUIInteractionBox> {
public:

    GameBoardElemLogic();
    virtual ~GameBoardElemLogic();

    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETGameBoardElemLogic
    void ET_setBoardPos(const Vec2i& pt) override;
    const Vec2i& ET_getBoardPos() const override;

    // ETUIInteractionBox
    void ET_onPress() override;
    void ET_onHover(bool flag) override;
    bool ET_isHovered() const override;
    AABB2Di ET_getHitBox() const override;

private:

    Vec2i boardPos;
};

#endif /* __GAME_BOARD_ELEMENT_LOGIC_HPP__ */