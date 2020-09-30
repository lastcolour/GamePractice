#ifndef __DEBUG_COMMANDS_HPP__
#define __DEBUG_COMMANDS_HPP__

#include "Math/AABB.hpp"
#include "Render/Color.hpp"

#include <string>

struct DebugDrawLine {
    Vec2i startPt;
    Vec2i endPt;
    ColorB startCol;
    ColorB endCol;
    int width;
};

struct DebugDrawQuad {
    AABB2Di box;
    ColorB col;
};

struct DebugDrawText {
    Vec2i pt;
    std::string text;
    ColorB col;
};

#endif /* __DEBUG_COMMANDS_HPP__ */