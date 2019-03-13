#ifndef __GAME_BOARD_LOGIC_HPP__
#define __GAME_BOARD_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "Math/AABB.hpp"
#include "ETApplicationInterfaces.hpp"

#include <vector>
#include <memory>

class GameObject;

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
    public ETNode<ETSurfaceEvents>,
    public ETNode<ETGameTick> {
public:

    GameBoardLogic();
    virtual ~GameBoardLogic();

    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETSurfaceEvents
    void ET_onSurfaceTouch(ETouchType touchType, const Vec2i& pt) override;
    void ET_onSurfaceResize(const Vec2i& pt) override;

    // ETGameTick
    void ET_onGameTick(float dt) override;

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
    ColorF getElemColor(BoardElemType color) const;
    void updateAfterRemoves();
    void updateBoard();

    void onStartElemMove();
    void onElemMove();
    void onEndElemMove(const Vec2i& pt);

    bool removeVerticalLine(const Vec2i& boardPt, int lineLen);
    bool removeHorizontalLine(const Vec2i& boardPt, int lineLen);

protected:

    std::vector<BoardElement> elements;
    std::string cellObject;
    AABB2D boardBox;
    Vec2i boardSize;
    float space;
    float cellScale;
    float cellSize;
    float moveSpeed;
    float objectSize;
    int activeTouchedElemId;
};

#endif /* __GAME_BOARD_LOGIC_HPP__ */