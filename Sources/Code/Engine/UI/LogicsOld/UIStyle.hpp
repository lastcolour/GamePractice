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

struct UIBoxStyle {
public:

    static void Reflect(ReflectContext& ctx);

public:

    Vec2 size;
    XAlignType xAlignType;
    YAlignType yAlignType;
    BoxMargin margin;

public:

    UIBoxStyle();
    ~UIBoxStyle();
};

struct UILabelStyle {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UILabelStyle();
    ~UILabelStyle();

public:

    ColorB color;
    float fontSize;
};

#endif /* __UI_STYLE_HPP__ */