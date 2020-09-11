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


    int getSizeOnGrind(float value) const;

public:

    int horizontalGrid;
    std::string mainView;
    std::string gameView;
    std::string backgroundView;
    std::string endGameView;
    std::string loadingView;
};

#endif /* __UI_CONFIG_HPP__ */