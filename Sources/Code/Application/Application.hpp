#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include <memory>

class Platform;
class ModuleFactory;
class Logger;
class Assets;
class Surface;
class ETSystem;
class Render;
class Game;

class Application {

    friend class Environment;

public:

    explicit Application(Platform* pltfrm);
    virtual ~Application();

    int run();

protected:

    virtual std::unique_ptr<ModuleFactory> createModuleFactory();
    virtual bool initLogger(Logger& logger);
    virtual bool initAssets(Assets& assets);
    virtual bool initSurface(Surface& surface);
    virtual bool initRender(Render& render);
    virtual bool initGame(Game& game);

protected:

    bool init();
    void mainLoop();

private:

    Application() = delete;
    Application(const Application& app) = delete;
    Application& operator=(const Application& app) = delete;

private:

    std::unique_ptr<ETSystem> etSystem;
    std::unique_ptr<Platform> platform;
    std::unique_ptr<Logger> logger;
    std::unique_ptr<Assets> assets;
    std::unique_ptr<Surface> surface;
    std::unique_ptr<Render> render;
    std::unique_ptr<Game> game;
};

#endif /* __APPLICATION_HPP__ */