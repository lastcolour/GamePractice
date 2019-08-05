#ifndef __UI_STYLE_HPP__
#define __UI_STYLE_HPP__

#include "Math/Vector.hpp"
#include "Render/Color.hpp"

#include <string>

class JSONNode;

enum class XAlignType {
    Center = 0,
    Left,
    Right
};

enum class YAlignType {
    Center = 0,
    Top,
    Bot
};

enum class SizeInvariant {
    Relative = 0,
    RelativeBiggestSquare,
    Absolute,
    AbsoluteBiggestSquare,
    Pixel
};

struct Margin {
    float left;
    float right;
    float bot;
    float top;

    Margin();
};

struct UIStyle {
public:

    Vec2 size;
    ColorB color;
    SizeInvariant sizeInv;
    XAlignType xAlignType;
    YAlignType yAlignType;
    Margin margin;
    std::string renderer;
    float fontSize;
    ColorB fontColor;

public:

    UIStyle();
    ~UIStyle();
    void serialize(const JSONNode& node);
};

#endif /* __UI_STYLE_HPP__ */