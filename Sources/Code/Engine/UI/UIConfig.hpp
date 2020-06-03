#ifndef __UI_CONFIG_HPP__
#define __UI_CONFIG_HPP__

#include <string>

class ReflectContext;

class UIConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIConfig();
    ~UIConfig();

public:

    int horizontalGrid;
};

#endif /* __UI_CONFIG_HPP__ */