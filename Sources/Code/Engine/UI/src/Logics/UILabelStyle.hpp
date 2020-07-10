#ifndef __UI_LABEL_STYLE_HPP__
#define __UI_LABEL_STYLE_HPP__

#include "UI/UIBoxStyle.hpp"


class UILabelStyle {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UILabelStyle();
    ~UILabelStyle();

public:

    UIBoxStyle::Margin margin;
    float fontSize;
};

#endif /* __UI_LABEL_STYLE_HPP__ */