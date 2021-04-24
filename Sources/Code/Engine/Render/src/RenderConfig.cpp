#include "RenderConfig.hpp"
#include "Reflect/EnumInfo.hpp"

void DebugTasks::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<DebugTasks>("DebugTasks")) {
        classInfo->addField("tasks", &DebugTasks::tasks);
    }
}

void FontConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<FontConfig>("FontConfig")) {
        classInfo->addField("file", &FontConfig::file);
        classInfo->addField("lerpType", &FontConfig::lerpType);
        classInfo->addField("size", &FontConfig::size);
    }
}

void CameraConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<CameraConfig>("CameraConfig")) {
        classInfo->addField("zNear", &CameraConfig::zNear);
        classInfo->addField("zFar", &CameraConfig::zFar);
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
        classInfo->addField("default_fonts", &RenderConfig::fontConfig);
        classInfo->addField("default_camera", &RenderConfig::cameraConfig);
        classInfo->addField("debug_tasks", &RenderConfig::debugTaskConfig);
        classInfo->addField("shaders_table", &RenderConfig::shadersTable);
    }
}
