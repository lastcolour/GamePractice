#ifndef __GAME_BOARD_LOGIC_HPP__
#define __GAME_BOARD_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Math/AABB.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/ETGameTimer.hpp"
#include "Game/Logics/GameBoardFSM.hpp"
#include "UI/UIProxyContainer.hpp"

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
    public ETNode<ETGameBoard> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardLogic();
    virtual ~GameBoardLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoard
    void ET_replaceElemToSpecial(EntityId targetId, EBoardElemType elemType) override;
    void ET_switchElemsBoardPos(EntityId firstId, EntityId secondId) override;
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
    void ET_resize(const AABB2D& newAabb) override;
    void ET_readBoardMatchState(BoardMatchState& boardMatchState) const override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

protected:

    virtual BoardElement createNewElement(const Vec2i& boardPt) const;

protected:

    Vec2i getBoardPosFromPos(const Vec2i& boardPt, const Vec3& pt) const;
    const BoardElement* getElem(EntityId entId) const;
    BoardElement* getElem(EntityId entId);
    BoardElement* getElem(const Vec2i& boardPt);
    const BoardElement* getElem(const Vec2i& boardPt) const;
    void setElemBoardPos(BoardElement& elem, const Vec2i& boardPt) const;
    void switchElements(int firstElem, int secondElem);
    void removeElem(BoardElement& elem);
    void respawnDestroyedElems();
    void processMovingElems(float dt);

protected:

    UIProxyContainer uiProxies;
    std::vector<std::vector<BoardElement>> columns;
    AABB2Di boardBox;
    Vec2i boardSize;
    Vec2 objectSize;
    EntityId backgroundId;
    float cellScale;
    float moveSpeed;
    float moveAccel;
    int cellSize;
    int elemsZOffset;
    int backgroundZOffset;
    GameBoardFSM gameBoardFSM;
};

#endif /* __GAME_BOARD_LOGIC_HPP__ */