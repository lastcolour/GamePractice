#ifndef __ENVIRONMENT_HPP__
#define __ENVIRONMENT_HPP__

#include "Application.hpp"

class Environment;
class Platform;
class Logger;
class Surface;
class Render;
class Game;

Environment* GetEnv();

class Environment {
private:

    friend class Application;
    friend Environment* GetEnv();

private:

    static Environment* ENV;

private:

    static bool InitEnv(Application& application);
    static void DeinitEnv();

private:

    Environment(Application& application);
    ~Environment();

public:

    Application& getApp();
    Platform& getPlatform();
    Logger* getLogger();
    Assets* getAssets();
    Surface* getSurface();
    Render* getRender();
    Game* getGame();

private:

    Application& app;
};

#endif /* __ENVIRONMENT_HPP__ */