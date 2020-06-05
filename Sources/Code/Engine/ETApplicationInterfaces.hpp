#ifndef __ET_APPLICATION_INTERFACES_HPP__
#define __ET_APPLICATION_INTERFACES_HPP__

#include "Core/StringFormat.hpp"
#include "Core/ETPrimitives.hpp"
#include "Math/Vector.hpp"

class ClassInstance;
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

enum class EActionType {
    Press = 0,
    Move,
    Release
};

enum class EButtonId {
    Back = 0,
};

struct ETInputEvents {
    virtual void ET_onTouch(EActionType actionType, const Vec2i& pt) = 0;
    virtual void ET_onButton(EActionType actionType, EButtonId buttonId) = 0;
};

struct ETSurfaceEvents {
    virtual ~ETSurfaceEvents() = default;
    virtual void ET_onSurfaceCreated() = 0;
    virtual void ET_onSurfaceDestroyed() = 0;
    virtual void ET_onSurfaceHidden() = 0;
    virtual void ET_onSurfaceShown() = 0;
    virtual void ET_onSurfaceLostFocus() = 0;
    virtual void ET_onSurfaceGainFocus() = 0;
    virtual void ET_onSurfaceResized(const Vec2i& size) = 0;
};

struct ETAssets {
    virtual ~ETAssets() = default;
    virtual Buffer ET_loadAsset(const char* assetName) = 0;
    virtual JSONNode ET_loadJSONAsset(const char* assetName) = 0;
    virtual Buffer ET_loadLocalFile(const char* fileName) = 0;
    virtual JSONNode ET_loadLocalJSONFile(const char* fileName) = 0;
    virtual bool ET_saveLocalFile(const char* fileName, const Buffer& buff) = 0;
    virtual bool ET_removeLocalFile(const char* fileName) = 0;
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
    virtual void ET_onPause() = 0;
    virtual void ET_onResume() = 0;
    virtual bool ET_isNeedRun() const = 0;
};

struct ETAssetsCacheManager {
    virtual ~ETAssetsCacheManager() = default;
    virtual Buffer ET_getAssetFromCache(const char* assetName) = 0;
    virtual void ET_putAssetToCache(const char* assetName, const Buffer& buff) = 0;
    virtual void ET_setCacheLifetime(float seconds) = 0;
    virtual float ET_getCacheLifetime() const = 0;
    virtual void ET_clear() = 0;
};

struct ETModuleConfigManager {
    virtual ~ETModuleConfigManager() = default;
    virtual void ET_registerConfig(ClassInstance& configInstance) = 0;
    virtual void* ET_getConfig(TypeId configId) = 0;
};

struct ETAssetsSetup {
    virtual ~ETAssetsSetup() = default;
    virtual void ET_getAssetsSearchPath(std::string& searchPath) = 0;
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

template<typename ConfigT>
ConfigT* ET_getConfig() {
    auto configId = GetTypeId<ConfigT>();
    void* moduleConfig = nullptr;
    ET_SendEventReturn(moduleConfig, &ETModuleConfigManager::ET_getConfig, configId);
    return reinterpret_cast<ConfigT*>(moduleConfig);
}

#endif /* __ET_APPLICATION_INTERFACES_HPP__ */