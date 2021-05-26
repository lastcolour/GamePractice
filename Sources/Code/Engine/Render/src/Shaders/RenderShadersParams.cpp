#include "Shaders/RenderShadersParams.hpp"
#include "Reflect/ReflectUtils.hpp"

void RenderShaderParams::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderShaderParams>("RenderShaderParams")) {
    }
}

RenderShaderParams::RenderShaderParams(EShaderType shaderType) :
    type(shaderType) {}

RenderShaderParams::~RenderShaderParams() {
}

void SolidColorShaderParams::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<SolidColorShaderParams>("SolidColorShaderParams")) {
        classInfo->addBaseClass<RenderShaderParams>();
        classInfo->addField("color", &SolidColorShaderParams::color);
    }
}

SolidColorShaderParams::SolidColorShaderParams() :
    RenderShaderParams(EShaderType::Solid),
    color(255, 125, 18) {
}

void GradientColorShaderParams::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GradientColorShaderParams>("GradientColorShaderParams")) {
        classInfo->addBaseClass<RenderShaderParams>();
        classInfo->addField("startCol", &GradientColorShaderParams::startCol);
        classInfo->addField("endCol", &GradientColorShaderParams::endCol);
        classInfo->addField("isVertical", &GradientColorShaderParams::isVertical);
    }
}

GradientColorShaderParams::GradientColorShaderParams() :
    RenderShaderParams(EShaderType::Gradient),
    startCol(255, 125, 18),
    endCol(0, 255, 138),
    isVertical(false) {
}

void ImageShaderParams::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ImageShaderParams>("ImageShaderParams")) {
        classInfo->addBaseClass<RenderShaderParams>();
        classInfo->addField("textureInfo", &ImageShaderParams::texInfo);
    }
}

ImageShaderParams::ImageShaderParams() :
    RenderShaderParams(EShaderType::Image) {
}

void TextShaderParams::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<TextShaderParams>("TextShaderParams")) {
        classInfo->addField("color", &TextShaderParams::color);
    }
}

TextShaderParams::TextShaderParams() :
    RenderShaderParams(EShaderType::Text),
    color(255, 255, 255) {
}

void ParticlesShaderParams::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ParticlesShaderParams>("ParticlesShaderParams")) {
        classInfo->addField("textureInfo", &ParticlesShaderParams::texInfo);
    }
}

ParticlesShaderParams::ParticlesShaderParams() :
    RenderShaderParams(EShaderType::Particles) {
}

void ReflectShadersParams() {
    ReflectUtils::CreateClassInfo<SolidColorShaderParams>();
    ReflectUtils::CreateClassInfo<GradientColorShaderParams>();
    ReflectUtils::CreateClassInfo<ImageShaderParams>();
    ReflectUtils::CreateClassInfo<TextShaderParams>();
    ReflectUtils::CreateClassInfo<ParticlesShaderParams>();
}