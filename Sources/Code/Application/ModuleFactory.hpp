#ifndef __MODULE_FACTORY_HPP__
#define __MODULE_FACTORY_HPP__

#include <memory>

class Logger;
class Assets;
class Surface;
class Render;
class Game;
class ETSystem;

class ModuleFactory {
public:

    ModuleFactory() = default;
    virtual ~ModuleFactory() = default;

    virtual std::unique_ptr<Logger> createLogger();
    virtual std::unique_ptr<Assets> createAssets();
    virtual std::unique_ptr<Surface> createSurface();
    virtual std::unique_ptr<Render> createRender();
    virtual std::unique_ptr<Game> createGame();
};

#endif /* __MODULE_FACTORY_HPP__ */