#ifndef __GAME_HPP__
#define __GAME_HPP__

class Game {
public:

    Game();
    virtual ~Game();

    virtual bool init();
    virtual bool shouldRun();
    virtual void update();
};

#endif /* __GAME_HPP__ */