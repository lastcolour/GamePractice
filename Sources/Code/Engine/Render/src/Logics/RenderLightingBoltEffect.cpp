#include "Logics/RenderLightingBoltEffect.hpp"

void RenderLightingBoltEffect::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderLightingBoltEffect>("RenderLightingBoltEffect")) {
    }
}

RenderLightingBoltEffect::RenderLightingBoltEffect() :
    RenderNode(RenderNodeType::LightingBolt) {
}

RenderLightingBoltEffect::~RenderLightingBoltEffect() {
}

void RenderLightingBoltEffect::init() {
}

void RenderLightingBoltEffect::deinit() {
}