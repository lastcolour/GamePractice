#ifndef __UI_STYLE_HPP__
#define __UI_STYLE_HPP__

#include "Math/Vector.hpp"
#include "Render/ETRenderInterfaces.hpp"

class JSONNode;

enum class AlignType {
    Center = 0,
    Left,
    Right
};

enum class SizeInvariant {
    Relative = 0,
    RelativeBiggestSquare,
    Absolute,
    AbsoluteBiggestSquare,
    Pixel
};

struct UIStyle {
public:

    Vec2 size;
    ColorB color;
    SizeInvariant sizeInv;
    AlignType alignType;

public:

    UIStyle();
    ~UIStyle();
    bool serialize(const JSONNode& node);
};

#endif /* __UI_STYLE_HPP__ */