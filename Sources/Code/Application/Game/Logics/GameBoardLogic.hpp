#ifndef __GAME_BOARD_LOGIC_HPP__
#define __GAME_BOARD_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Math/AABB.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "Entity/ETEntityInterfaces.hpp"

#include <vector>
#include <memory>

enum class BoardElemType {
    Red = 0,
    Blue,
    Green,
    Purple,
    Yellow,
    ENUM_COUNT
};

struct BoardElement {
    AABB2Di box;
    Vec2i movePt;
    Vec2i boardPt;
    EntityId entId;
    BoardElemType color;
    EBoardElemState state;
};

class GameBoardLogic : public EntityLogic,
    public ETNode<ETGameTimerEvents>,
    public ETNode<ETUIBoxEvents>,
    public ETNode<ETEntityEvents>,
    public ETNode<ETGameBoard> {
public:

    GameBoardLogic();
    virtual ~GameBoardLogic();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETGameBoard
    void ET_switchElemsBoardPos(EntityId firstId, EntityId secondId) override;
    void ET_setElemState(EntityId elemEntId, EBoardElemState newState) override;
    EBoardElemState ET_getElemState(EntityId elemEntId) const override;
    void ET_updateBoard() override;
    EntityId ET_getElemByPos(const Vec2i& pt) const override;
    EntityId ET_getElemByBoardPos(const Vec2i& boardPt) const override;
    Vec2i ET_getElemBoardPos(EntityId elemEntId) const override;
    int ET_getCellSize() const override;
    const Vec2i& ET_getBoardSize() const override;
    const AABB2Di& ET_getBoardBox() const override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

    // ETUIBoxEvents
    void ET_onBoxResized() override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override;
    void ET_onChildAdded(EntityId childId) override { (void)childId; }

protected:

    virtual BoardElemType getElemType() const;
    int getElemId(const Vec2i& boardPt) const;
    Vec2i getBoardPosFromPos(const Vec2i& boardPt, const Vec3& pt) const;
    Vec3 getPosFromBoardPos(const Vec2i& boardPt) const;
    const BoardElement* getElem(EntityId entId) const;
    BoardElement* getElem(EntityId entId);
    BoardElement* getElem(const Vec2i& boardPt);
    const BoardElement* getElem(const Vec2i& boardPt) const;
    int getVoidElemBelow(const Vec2i& boardPt) const;
    const BoardElement* getTopElem(const Vec2i& boardPt) const;
    bool moveElem(BoardElement& elem, float dt);
    void markForRemoveElems(const std::vector<int>& elems);
    bool isElemMatch(int firstElemId, int secondElemId) const;
    void initNewElem(BoardElement& elem, const Vec2i& boardPt) const;
    void setElemBoardPos(BoardElement& elem, const Vec2i& boardPt) const;
    void switchElements(int firstElem, int secondElem);
    ColorB getElemColor(BoardElemType color) const;
    void updateAfterRemoves();
    void updateBoard();

    bool removeVerticalLine(const Vec2i& boardPt, int lineLen);
    bool removeHorizontalLine(const Vec2i& boardPt, int lineLen);

    void initBoardBox();

protected:

    std::vector<BoardElement> elements;
    std::string cellObject;
    AABB2Di boardBox;
    Vec2i boardSize;
    Vec2i objectSize;
    float cellScale;
    float moveSpeed;
    int cellSize;
    int activeTouchedElemId;
    bool doUpdate;
};

#endif /* __GAME_BOARD_LOGIC_HPP__ */