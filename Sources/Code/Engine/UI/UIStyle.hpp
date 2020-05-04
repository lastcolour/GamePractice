#ifndef __UI_STYLE_HPP__
#define __UI_STYLE_HPP__

#include "Math/Vector.hpp"
#include "Render/Color.hpp"

#include <string>

class ReflectContext;

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

class BoxMargin {
public:

    static void Reflect(ReflectContext& ctx);

public:

    BoxMargin();

public:

    float left;
    float right;
    float bot;
    float top;
};

struct UIStyle {
public:

    static void Reflect(ReflectContext& ctx);

public:

    Vec2 size;
    ColorB color;
    SizeInvariant sizeInv;
    XAlignType xAlignType;
    YAlignType yAlignType;
    BoxMargin margin;
    std::string renderer;
    float fontSize;
    ColorB fontColor;

public:

    UIStyle();
    ~UIStyle();
};

#endif /* __UI_STYLE_HPP__ */