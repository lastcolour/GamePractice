#ifndef __RENDER_CONFIG_HPP__
#define __RENDER_CONFIG_HPP__

#include "Render/TextureInfo.hpp"

class DebugTasks {
public:

    static void Reflect(ReflectContext& ctx);

public:

    std::vector<std::string> tasks;
};

class FontConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    std::string file;
    int size;
    ETextureLerpType lerpType;
};

class CameraConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    float zNear;
    float zFar;
};

class RenderConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    FontConfig fontConfig;
    CameraConfig cameraConfig;
    DebugTasks debugTaskConfig;
};

#endif /* __RENDER_CONFIG_HPP__ */