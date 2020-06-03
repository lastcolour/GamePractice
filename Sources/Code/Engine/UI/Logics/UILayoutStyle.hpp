#ifndef __UI_LAYOUT_STYLE_HPP__
#define __UI_LAYOUT_STYLE_HPP__

class ReflectContext;

enum class UIXAlign {
    Left = 0,
    Center,
    Right
};

enum class UIYAlign {
    Top = 0,
    Center,
    Bot
};

enum class UILayoutType {
    Vertical = 0,
    Horizontal
};

class UILayoutStyle {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UILayoutStyle();
    ~UILayoutStyle();

public:

    UILayoutType type;
    UIXAlign xAlign;
    UIYAlign yAlign;
};

#endif /* __UI_LAYOUT_STYLE_HPP__ */