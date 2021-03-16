#ifndef __UI_VIEW_CONFIG_HPP__
#define __UI_VIEW_CONFIG_HPP__

class UIViewConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIViewConfig();
    ~UIViewConfig();

public:

    std::string mainView;
    std::string gameView;
    std::string backgroundView;
    std::string endGameView;
    std::string loadingView;
    std::string pauseView;
    std::string levelsView;
};

#endif /* __UI_VIEW_CONFIG_HPP__ */