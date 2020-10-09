#ifndef __DEBUG_COMMANDS_HPP__
#define __DEBUG_COMMANDS_HPP__

#include "Math/AABB.hpp"
#include "Render/Color.hpp"

#include <string>

struct DebugDrawLineCmd {
    Vec2i startPt;
    Vec2i endPt;
    ColorB startCol;
    ColorB endCol;
    int width;
};

struct DebugDrawQuadCmd {
    AABB2Di box;
    ColorB col;
};

struct DebugDrawTextCmd {
    Vec2i pt;
    std::string text;
    float size;
    ColorB col;
};

#endif /* __DEBUG_COMMANDS_HPP__ */