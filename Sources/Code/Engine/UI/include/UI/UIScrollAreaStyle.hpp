#ifndef __UI_SCROLL_AREA_STYLE_HPP__
#define __UI_SCROLL_AREA_STYLE_HPP__

enum class UIScrollType {
    Vertical = 0,
    Horizontal
};

enum class UIScrollOrigin {
    Start = 0,
    End
};

class UIScrollAreaStyle {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIScrollAreaStyle();
    ~UIScrollAreaStyle();

public:

    UIScrollType type;
    UIScrollOrigin origin;
};

#endif /* __UI_SCROLL_AREA_STYLE_HPP__ */