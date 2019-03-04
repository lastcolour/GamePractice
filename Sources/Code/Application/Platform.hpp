#ifndef __PLATFORM_HPP__
#define __PLATFORM_HPP__

#include <memory>

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"

class Logger : public SystemLogic,
    public ETNode<ETLogger> {
public:

    virtual ~Logger() = default;

protected:

    virtual void printMessasge(LogLevel logLevel, const std::string& msg) = 0;
    virtual std::string formatMessage(LogLevel logLevel, const std::string& msg) = 0;
};

class Assets : public SystemLogic,
    public ETNode<ETAsset> {
public:

    virtual ~Assets() = default;
};

class Surface : public SystemLogic,
    public ETNode<ETSurface> {
public:

    virtual ~Surface() = default;
};

class Platform {
public:

    virtual ~Platform() = default;
    virtual bool init() = 0;

    virtual std::unique_ptr<Surface> createSurface() = 0;
    virtual std::unique_ptr<Logger> createLogger() = 0;
    virtual std::unique_ptr<Assets> createAssets() = 0;
};

#endif /* __PLATFORM_HPP__ */