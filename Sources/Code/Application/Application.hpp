#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include <memory>

class Platform;
class Game;

class Application {
public:

    Application(Platform* pltfrm);
    virtual ~Application();

    int run();

protected:

    virtual std::unique_ptr<Game> createGame();

private:

    void mainLoop();

private:

    std::unique_ptr<Platform> platform;
    std::unique_ptr<Game> game;
};

#endif /* __APPLICATION_HPP__ */