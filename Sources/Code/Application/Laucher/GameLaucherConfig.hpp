#ifndef __GAME_LAUCHER_CONFIG_HPP__
#define __GAME_LAUCHER_CONFIG_HPP__

#include <string>

class ReflectContext;

struct GameLaucherConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameLaucherConfig();
    virtual ~GameLaucherConfig();

public:

    std::string mainView;
};

#endif /* __GAME_LAUCHER_CONFIG_HPP__ */