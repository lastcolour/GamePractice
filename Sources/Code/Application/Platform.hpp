#ifndef __PLATFORM_HPP__
#define __PLATFORM_HPP__

#include <memory>

class Surface;
class LogStream;
class Assets;

class Platform {
public:

    virtual ~Platform() = default;
    virtual bool init() = 0;

    virtual std::unique_ptr<Surface> createSurface() = 0;
    virtual std::unique_ptr<LogStream> createLogStream() = 0;
    virtual std::unique_ptr<Assets> createAssets() = 0;
};

#endif /* __PLATFORM_HPP__ */