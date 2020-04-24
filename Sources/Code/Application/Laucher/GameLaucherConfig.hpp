#ifndef __GAME_LAUCHER_CONFIG_HPP__
#define __GAME_LAUCHER_CONFIG_HPP__

#include <string>

class JSONNode;

struct GameLaucherConfig {
public:

    GameLaucherConfig();
    virtual ~GameLaucherConfig();

    void serialize(const JSONNode& node);

public:

    std::string mainView;
    std::string background;
};

#endif /* __GAME_LAUCHER_CONFIG_HPP__ */