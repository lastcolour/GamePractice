#ifndef __GAME_LOGIC_HPP__
#define __GAME_LOGIC_HPP__

class JSONNode;

class GameLogic {
public:

    virtual ~GameLogic() = default;

    virtual bool init(const JSONNode& node) = 0;
    virtual void update() = 0;
};

#endif /* __GAME_LOGIC_HPP__ */