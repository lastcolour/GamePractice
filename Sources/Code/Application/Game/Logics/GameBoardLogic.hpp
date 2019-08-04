#ifndef __GAME_BOARD_LOGIC_HPP__
#define __GAME_BOARD_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "Math/AABB.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "UI/UIETInterfaces.hpp"

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

enum class BoardElemState {
    Static = 0,
    Moving,
    Void
};

struct BoardElement {
    AABB2Di box;
    Vec2i boardPt;
    Vec2i movePt;
    EntityId entId;
    BoardElemType color;
    BoardElemState state;
};

class GameBoardLogic : public GameLogic,
    public ETNode<ETInputEvents>,
    public ETNode<ETTimerEvents>,
    public ETNode<ETUIBoxEvents>,
    public ETNode<ETGameObjectEvents> {
public:

    GameBoardLogic();
    virtual ~GameBoardLogic();

    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETInputEvents
    void ET_onTouch(ETouchType touchType, const Vec2i& pt) override;

    // ETTimerEvents
    void ET_onTick(float dt) override;

    // ETUIBoxEvents
    void ET_onBoxResized() override;

    // ETGameObjectEvents
    void ET_onTransformChanged(const Transform& newTm) override;
    void ET_onChildAdded(EntityId childId) override { (void)childId; }

protected:

    virtual BoardElemType getElemType() const;
    int getElemId(const Vec2i& boardPt) const;
    Vec2i getBoardPosFromPos(const Vec2i& boardPt, const Vec3& pt) const;
    Vec3 getPosFromBoardPos(const Vec2i& boardPt) const;
    BoardElement* getElem(const Vec2i& boardPt);
    const BoardElement* getElem(const Vec2i& boardPt) const;
    int getVoidElemBelow(const Vec2i& boardPt) const;
    const BoardElement* getTopElem(const Vec2i& boardPt) const;
    bool moveElem(BoardElement& elem, float dt);
    void markForRemoveElems(const std::vector<int>& elems);
    bool isElemMatch(int firstElemId, int secondElemId) const;
    int findTouchedElemId(const Vec2i& pt) const;
    void initNewElem(BoardElement& elem, const Vec2i& boardPt) const;
    void setElemBoardPos(BoardElement& elem, const Vec2i& boardPt) const;
    void switchElements(int firstElem, int secondElem);
    ColorB getElemColor(BoardElemType color) const;
    void updateAfterRemoves();
    void updateBoard();

    void onStartElemMove();
    void onElemMove();
    void onEndElemMove(const Vec2i& pt);

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
};

#endif /* __GAME_BOARD_LOGIC_HPP__ */