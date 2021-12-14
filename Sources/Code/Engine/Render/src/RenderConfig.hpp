#ifndef __RENDER_CONFIG_HPP__
#define __RENDER_CONFIG_HPP__

#include "Render/RenderCommon.hpp"

class DebugTasks {
public:

    static void Reflect(ReflectContext& ctx);

public:

    std::vector<std::string> tasks;
};

class FontDescription {
public:

    static void Reflect(ReflectContext& ctx);

public:

    std::string file;
    int size;
    ETextureLerpType lerpType;
    bool monochrome;
};

class FontsConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    FontDescription gameFont;
    FontDescription debugFont;
};

class CameraConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    float zNear;
    float zFar;
};

class ParticlesConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    int maxParticles;
};

class RenderConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    FontsConfig fontsConfig;
    CameraConfig cameraConfig;
    DebugTasks debugTaskConfig;
    ParticlesConfig particlesConfig;
    std::string shadersTable;
};

#endif /* __RENDER_CONFIG_HPP__ */