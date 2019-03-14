#ifndef __UI_STYLE_HPP__
#define __UI_STYLE_HPP__

#include "Math/Vector.hpp"

class JSONNode;

enum class AlignType {
    Center = 0,
    Left,
    Right
};

struct UIStyle {
public:

    AlignType alignType;
    Vec2 size;

public:

    UIStyle();
    ~UIStyle();
    bool serialize(const JSONNode& node);
};

#endif /* __UI_STYLE_HPP__ */