#ifndef __DEBUG_COMMANDS_HPP__
#define __DEBUG_COMMANDS_HPP__

#include "Math/AABB.hpp"
#include "Render/Color.hpp"

struct DebugDrawQuadCmd {
    AABB2D box;
    ColorB col;
};

struct DebugDrawTextCmd {
    Vec2 pt;
    std::string text;
    float size;
    ColorB col;
};

#endif /* __DEBUG_COMMANDS_HPP__ */