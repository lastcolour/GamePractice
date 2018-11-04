#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include <memory>

class Platform;

class Application {
public:

    Application(Platform* pltfrm);
    ~Application();

    int run();

private:

    void mainLoop();

private:

    std::unique_ptr<Platform> platform;
};

#endif /* __APPLICATION_HPP__ */