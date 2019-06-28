#ifndef __ET_APPLICATION_INTERFACES_HPP__
#define __ET_APPLICATION_INTERFACES_HPP__

#include "Core/StringFormat.hpp"
#include "Core/ETPrimitives.hpp"
#include "Math/Vector.hpp"

class JSONNode;

enum GLContextType {
    None = 0,
    ES20,
    ES30
};

struct ETSurface {
    virtual ~ETSurface() = default;
    virtual bool ET_show() = 0;
    virtual bool ET_hide() = 0;
    virtual void ET_close() = 0;
    virtual Vec2i ET_getSize() const = 0;
    virtual void ET_swapBuffers() = 0;
    virtual GLContextType ET_getGLContextType() const = 0;
    virtual bool ET_isVisible() const = 0;
    virtual bool ET_isValid() const = 0;
};

enum class ETouchType {
    Press = 0,
    Move,
    Release
};

struct ETSurfaceEvents {
    virtual ~ETSurfaceEvents() = default;
    virtual void ET_onSurfaceTouch(ETouchType touchType, const Vec2i& pt) = 0;
    virtual void ET_onSurfaceResize(const Vec2i& size) = 0;
};

struct ETAssets {
    virtual ~ETAssets() = default;
    virtual Buffer ET_loadAsset(const std::string& assetName) = 0;
    virtual JSONNode ET_loadJSONAsset(const std::string& assetName) = 0;
};

enum class LogLevel {
    Debug = 0,
    Info,
    Warning,
    Error,
    Fatal,
    Silent
};

struct ETLogger {
    virtual ~ETLogger() = default;
    virtual void ET_logMessage(LogLevel lvl, const std::string& msg) = 0;
    virtual void ET_setLogLevel(LogLevel lvl) = 0;
};

struct ETTimer {
    virtual ~ETTimer() = default;
    virtual void ET_onFrameStart() = 0;
};

struct ETTimerEvents {
    virtual ~ETTimerEvents() = default;
    virtual void ET_onTick(float dt) = 0;
};

struct ETAppRunStateEvents {
    virtual ~ETAppRunStateEvents() = default;
    virtual void ET_onTerminate() = 0;
    virtual bool ET_isNeedRun() const = 0;
};

template<typename ... ArgsT>
void LogDebug(const char* msg, const ArgsT& ... args) {
    ET_SendEvent(&ETLogger::ET_logMessage, LogLevel::Debug, StringFormat(msg, args...));
}

template<typename ... ArgsT>
void LogInfo(const char* msg, const ArgsT& ... args) {
    ET_SendEvent(&ETLogger::ET_logMessage, LogLevel::Info, StringFormat(msg, args...));
}

template<typename ... ArgsT>
void LogWarning(const char* msg, const ArgsT& ... args) {
    ET_SendEvent(&ETLogger::ET_logMessage, LogLevel::Warning, StringFormat(msg, args...));
}

template<typename ... ArgsT>
void LogError(const char* msg, const ArgsT& ... args) {
    ET_SendEvent(&ETLogger::ET_logMessage, LogLevel::Error, StringFormat(msg, args...));
}

template<typename ... ArgsT>
void LogFatal(const char* msg, const ArgsT& ... args) {
    ET_SendEvent(&ETLogger::ET_logMessage, LogLevel::Fatal, StringFormat(msg, args...));
}

#endif /* __ET_APPLICATION_INTERFACES_HPP__ */