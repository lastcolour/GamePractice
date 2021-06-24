#include "RenderConfig.hpp"
#include "Reflect/EnumInfo.hpp"

void DebugTasks::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<DebugTasks>("DebugTasks")) {
        classInfo->addField("tasks", &DebugTasks::tasks);
    }
}

void FontDescription::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<FontDescription>("FontDescription")) {
        classInfo->addField("file", &FontDescription::file);
        classInfo->addField("lerpType", &FontDescription::lerpType);
        classInfo->addField("size", &FontDescription::size);
        classInfo->addField("monochrome", &FontDescription::monochrome);
    }
}

void FontsConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<FontsConfig>("FontsConfig")) {
        classInfo->addField("game_font", &FontsConfig::gameFont);
        classInfo->addField("debug_font", &FontsConfig::debugFont);
    }
}

void CameraConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<CameraConfig>("CameraConfig")) {
        classInfo->addField("zNear", &CameraConfig::zNear);
        classInfo->addField("zFar", &CameraConfig::zFar);
    }
}

void ParticlesConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticlesConfig>("ParticlesConfig")) {
        classInfo->addField("max_particles", &ParticlesConfig::maxParticles);
    }
}

void RenderConfig::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<ETextureLerpType>("ETextureLerpType")) {
        enumInfo->addValues<ETextureLerpType>({
            {"Point", ETextureLerpType::Point},
            {"Linear", ETextureLerpType::Linear}
        });
    }
    if(auto classInfo = ctx.classInfo<RenderConfig>("RenderConfig")) {
        classInfo->addField("fonts", &RenderConfig::fontsConfig);
        classInfo->addField("default_camera", &RenderConfig::cameraConfig);
        classInfo->addField("debug_tasks", &RenderConfig::debugTaskConfig);
        classInfo->addField("particles", &RenderConfig::particlesConfig);
        classInfo->addField("shaders_table", &RenderConfig::shadersTable);
    }
}
