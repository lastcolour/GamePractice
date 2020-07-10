#ifndef __UI_BOX_STYLE_HPP__
#define __UI_BOX_STYLE_HPP__

#include "Math/Vector.hpp"

class ReflectContext;

enum class UIBoxSizeInvariant {
    Grid = 0,
    Relative
};

class UIBoxStyle {
public:

    class Margin {
    public:

        static void Reflect(ReflectContext& ctx);

    public:

        Margin() : top(0.f), bot(0.f), left(0.f), right(0.f) {}

    public:

        float top;
        float bot;
        float left;
        float right;
    };

public:

    static void Reflect(ReflectContext& ctx);

public:

    UIBoxStyle();
    ~UIBoxStyle();

public:

    float width;
    float height;
    UIBoxSizeInvariant widthInv;
    UIBoxSizeInvariant heightInv;
    Margin margin;
};

class UIBoxMargin {
public:

    UIBoxMargin();
    ~UIBoxMargin();

public:

    int top;
    int bot;
    int left;
    int right;
};

#endif /* __UI_BOX_STYLE_HPP__ */