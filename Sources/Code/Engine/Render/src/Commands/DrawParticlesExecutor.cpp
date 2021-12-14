#include "Commands/DrawParticlesExecutor.hpp"
#include "Render/ETRenderManager.hpp"
#include "RenderUtils.hpp"
#include "Render/ETRenderTickManager.hpp"

#include <cassert>

namespace {

float getUpdateDeltaTime(const ParticlesEmitterEmissionConfig& emissionConfig, float renderDt) {
    float dt = 0.f;
    switch(emissionConfig.emitterSimTime) {
        case EmitterSimulationTime::Render: {
            dt = renderDt;
            break;
        }
        case EmitterSimulationTime::Game: {
            ET_SendEventReturn(dt, &ETRenderTickManager::ET_getUIDeltaT);
            break;
        }
        case EmitterSimulationTime::UI: {
            ET_SendEventReturn(dt, &ETRenderTickManager::ET_getUIDeltaT);
            break;
        }
        default: {
            assert(false && "Invalid simulation time");
        }
    }
    return dt;
}

} // namespace

DrawParticlesExecutor::DrawParticlesExecutor() {
}

DrawParticlesExecutor::~DrawParticlesExecutor() {
}

bool DrawParticlesExecutor::init() {
    ET_SendEventReturn(shader, &ETRenderShaderManager::ET_createShader, "particle");
    if(!shader) {
        return false;
    }
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Particles);
    if(!geom) {
        return false;
    }
    return true;
}

void DrawParticlesExecutor::deinit() {
}

void DrawParticlesExecutor::preDraw() {
    for(size_t i = 0; i < visibleCount; ++i) {
        
    }
}

void DrawParticlesExecutor::update(float renderDt) {
    for(size_t i = 0; i < visibleCount; ++i) {
        auto particlesCmd = static_cast<DrawParticlesCmd*>(queue[i]);
        // auto& tm = particlesCmd->modelMat;
        Transform tm;
        auto& simConfig = particlesCmd->emittersPool.getSimConfig();

        auto deltaT = getUpdateDeltaTime(simConfig.emission, renderDt);
        particlesCmd->emittersPool.simulate(tm, deltaT);

        auto updateInfo = particlesCmd->emittersPool.getAndResetUpdateInfo();
        DispatchParticlesSystemEvents(getEntityId(), updateInfo);
    }
}

void DrawParticlesExecutor::draw(RenderState& renderState, DrawCmdSlice& slice) {
    geom->bind();
    shader->bind();
    shader->setUniformMat4(UniformType::CameraMat, renderState.proj2dMat);

    for(size_t i = slice.startIdx; i < slice.endIdx; ++i) {
        auto& cmd = *queue[i];
        renderState.startCommand(cmd);

        shader->setTexture2d(UniformType::Texture, 0, *cmd.texObj,
            cmd.renderConfig.textureInfo);

        for(auto& emitter : cmd.emittersPool.getEmitters()) {
            if(emitter->activeCount <= 0) {
                continue;
            }
            Mat4 resMat = emitter->getTransformMat();
            shader->setUniformMat4(UniformType::ModelMat, resMat);
            geom->drawInstanced(&cmd.emittersPool->instaceData[0], emitter->activeCount);
        }
    }

    shader->unbind();
    geom->unbind();
}