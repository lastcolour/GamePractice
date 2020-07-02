#ifndef __RENDER_CONFIG_HPP__
#define __RENDER_CONFIG_HPP__

#include <string>

class ReflectContext;

class RenderConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderConfig();

public:

    std::string defaultFont;
    int defaultFontSize;
};

#endif /* __RENDER_CONFIG_HPP__ */