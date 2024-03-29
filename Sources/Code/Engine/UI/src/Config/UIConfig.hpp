#ifndef __UI_CONFIG_HPP__
#define __UI_CONFIG_HPP__

#include "UIViewConfig.hpp"

class UIConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIConfig();
    ~UIConfig();

    float getSizeOnGrid(float value) const;
    float convertFromGrid(float value) const;

public:

    Vec2i baseRatio;
    int horizontalGrid;
    UIViewConfig views;
};

#endif /* __UI_CONFIG_HPP__ */