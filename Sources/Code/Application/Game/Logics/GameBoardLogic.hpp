#ifndef __GAME_BOARD_LOGIC_HPP__
#define __GAME_BOARD_LOGIC_HPP__

#include "Core/ETPrimitives.hpp"
#include "Entity/EntityLogic.hpp"
#include "Math/AABB.hpp"
#include "Game/ETGameBoard.hpp"
#include "UI/ETUIBox.hpp"
#include "Entity/ETEntity.hpp"
#include "Game/ETGameTimer.hpp"
#include "Core/ETPrimitives.hpp"

#include <vector>
#include <memory>

struct BoardElement {
    Vec2i boardPt;
    EntityId entId;
    float vel;

    BoardElement() :
        boardPt(-1),
        vel(0.f) {}
};

class GameBoardLogic : public EntityLogic,
    public ETNode<ETGameTimerEvents>,
    public ETNode<ETGameBoard>,
    public ETNode<ETUIElementEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardLogic();
    virtual ~GameBoardLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoard
    void ET_switchElemsBoardPos(EntityId firstId, EntityId secondId) override;
    void ET_matchElements() override;
    void ET_setBlockElemMatching(bool flag) override;
    EntityId ET_getElemByPos(const Vec2i& pt) const override;
    EntityId ET_getElemByBoardPos(const Vec2i& boardPt) const override;
    Vec2i ET_getElemBoardPos(EntityId elemEntId) const override;
    int ET_getCellSize() const override;
    const Vec2i& ET_getBoardSize() const override;
    const AABB2Di& ET_getBoardBox() const override;
    Vec3 ET_getPosFromBoardPos(const Vec2i& boardPt) const override;
    void ET_setUIElement(EntityId rootUIElementId) override;
    bool ET_isAllElemStatic() const override;

    // ETUIElementEvents
    void ET_onBoxChanged(const AABB2Di& newAabb) override;
    void ET_onZIndexChanged(int newZIndex) override;
    void ET_onAlphaChanged(float newAlpha) override;
    void ET_onHidden(bool flag) override;
    void ET_onDisabled(bool flag) override {}
    void ET_onIngoreTransform(bool flag) override {}

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

protected:

    Vec2i getBoardPosFromPos(const Vec2i& boardPt, const Vec3& pt) const;
    const BoardElement* getElem(EntityId entId) const;
    BoardElement* getElem(EntityId entId);
    BoardElement* getElem(const Vec2i& boardPt);
    const BoardElement* getElem(const Vec2i& boardPt) const;
    void setElemBoardPos(BoardElement& elem, const Vec2i& boardPt) const;
    void switchElements(int firstElem, int secondElem);
    void updateAfterRemoves();
    void matchElements();
    BoardElement createNewElement(const Vec2i& boardPt) const;
    void removeElem(BoardElement& elem);

protected:

    std::vector<std::vector<BoardElement>> columns;
    AABB2Di boardBox;
    Vec2i boardSize;
    Vec2 objectSize;
    EntityId uiBoxId;
    EntityId backgroundId;
    float cellScale;
    float moveSpeed;
    float moveAccel;
    int cellSize;
    int zBackgroundIndex;
    bool isElemMatchRequested;
    bool isBoardStatic;
    bool isElemMatchingBlocked;
};

#endif /* __GAME_BOARD_LOGIC_HPP__ */