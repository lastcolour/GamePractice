#ifndef __GAME_INITER_CONFIGS_HPP__
#define __GAME_INITER_CONFIGS_HPP__

#include <string>

class JSONNode;

struct GameIniterConfig {
public:

    GameIniterConfig();
    virtual ~GameIniterConfig();

    void serialize(const JSONNode& node);

public:

    std::string mainView;
    std::string background;
};

#endif /* __GAME_INITER_CONFIGS_HPP__ */