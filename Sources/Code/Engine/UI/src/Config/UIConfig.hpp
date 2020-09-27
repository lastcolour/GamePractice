#ifndef __UI_CONFIG_HPP__
#define __UI_CONFIG_HPP__

#include "UIViewConfig.hpp"

class UIConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIConfig();
    ~UIConfig();


    int getSizeOnGrind(float value) const;
    float convertFromGrid(int value) const;

public:

    int verticalGrid;
    UIViewConfig views;
};

#endif /* __UI_CONFIG_HPP__ */