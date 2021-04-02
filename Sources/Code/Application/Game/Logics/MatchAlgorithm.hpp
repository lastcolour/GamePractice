#ifndef __MATCH_ALGORITHM_HPP__
#define __MATCH_ALGORITHM_HPP__

#include "Game/ETGameElem.hpp"

enum class EPatterType {
    HLine = 0,
    VLine,
    HRocket,
    VRocket,
    Bomb,
    Star,
    Remained
};

struct MatchPoints {
    Vec2i pt;
    EntityId entId;
    EBoardElemType elemType;
};

struct PatternMatch {
    EPatterType patterType;
    EBoardElemType elemsType;
    std::vector<const MatchPoints*> points;
};

class BoardMatchState {
public:

    BoardMatchState();
    ~BoardMatchState();

    void reset();

    void setSize(const Vec2i& newSize);
    const Vec2i& getSize() const;

    void setElem(const Vec2i& pt, EntityId elemId);
    const MatchPoints& getElem(int x, int y) const;
    MatchPoints& getElem(int x, int y);

private:

    Vec2i size;
    std::vector<MatchPoints> elems;
};

std::vector<PatternMatch> FindAllMatchPatterns(const BoardMatchState& board);

#endif /* __MATCH_ALGORITHM_HPP__ */