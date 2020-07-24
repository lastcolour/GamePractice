#ifndef __GAME_BOARD_LOGIC_HPP__
#define __GAME_BOARD_LOGIC_HPP__

#include "Core/ETPrimitives.hpp"
#include "Entity/EntityLogic.hpp"
#include "Math/AABB.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "UI/ETUIBox.hpp"
#include "Entity/ETEntity.hpp"
#include "Game/ETGame.hpp"
#include "Core/ETPrimitives.hpp"

#include <vector>
#include <memory>

struct BoardElement {
    AABB2Di box;
    Vec2i movePt;
    Vec2i boardPt;
    EntityId entId;
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
    bool init() override;
    void deinit() override;

    // ETGameBoard
    void ET_switchElemsBoardPos(EntityId firstId, EntityId secondId) override;
    void ET_updateBoard() override;
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
    void ET_onBoxResized(const AABB2Di& newAabb) override;
    void ET_onZIndexChanged(int newZIndex) override;
    void ET_onAlphaChanged(float newAlpha) override;
    void ET_onHidden(bool flag) override;
    void ET_onDisabled(bool flag) override;
    void ET_onIngoreTransform(bool flag) override {}

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

protected:

    int getElemId(const Vec2i& boardPt) const;
    Vec2i getBoardPosFromPos(const Vec2i& boardPt, const Vec3& pt) const;
    const BoardElement* getElem(EntityId entId) const;
    BoardElement* getElem(EntityId entId);
    BoardElement* getElem(const Vec2i& boardPt);
    const BoardElement* getElem(const Vec2i& boardPt) const;
    int getVoidElemBelow(const Vec2i& boardPt) const;
    const BoardElement* getTopElem(const Vec2i& boardPt) const;
    bool moveElem(BoardElement& elem, float dt);
    void setElemBoardPos(BoardElement& elem, const Vec2i& boardPt) const;
    void switchElements(int firstElem, int secondElem);
    void updateAfterRemoves();
    void updateBoard();
    void setCellObject(const char* cellObjectName);
    bool createNewElement(const Vec2i& boardPt);

protected:

    virtual void setRandomElemType(BoardElement& elem) const;

protected:

    std::vector<BoardElement> elements;
    std::string cellObject;
    AABB2Di boardBox;
    Vec2i boardSize;
    Vec2i objectSize;
    EntityId uiBoxId;
    float cellScale;
    float moveSpeed;
    int cellSize;
    bool doUpdate;
};

#endif /* __GAME_BOARD_LOGIC_HPP__ */