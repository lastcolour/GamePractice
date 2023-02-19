#include "Commands/DrawParticlesExecutor.hpp"
#include "Render/ETRenderManager.hpp"
#include "Render/ETRenderTickManager.hpp"

#include <cassert>

namespace {

float getUpdateDeltaTime(const ParticlesEmitterEmissionConfig& emissionConfig, const ETRenderTickManager::Ticks& ticks) {
    switch(emissionConfig.emitterSimTime) {
        case EmitterSimulationTime::Render: {
            return ticks.renderDt;
        }
        case EmitterSimulationTime::Game: {
            return ticks.gameDt;
        }
        case EmitterSimulationTime::UI: {
            return ticks.uiDt;
        }
        default: {
            assert(false && "Invalid simulation time");
        }
    }
    return 0.f;
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

void DrawParticlesExecutor::registerCmdForDraw(DrawCmd* cmd) {
    DrawCommandExecutor<DrawParticlesCmd>::registerCmdForDraw(cmd);

    auto particlesCmd = static_cast<DrawParticlesCmd*>(cmd);
    particlesCmd->updateTexture(particlesCmd->renderConfig.textureInfo);
}

void DrawParticlesExecutor::preDraw(RenderState& renderState) {
    DrawCommandExecutor<DrawParticlesCmd>::preDraw(renderState);

    ETRenderTickManager::Ticks ticks;
    ET_SendEventReturn(ticks, &ETRenderTickManager::ET_getTicks);

    for(size_t i = 0; i < visibleCount; ++i) {
        auto particlesCmd = static_cast<DrawParticlesCmd*>(queue[i]);
        auto& simConfig = particlesCmd->emittersPool.getSimConfig();

        auto deltaT = getUpdateDeltaTime(simConfig.emission, ticks);
        particlesCmd->emittersPool.simulate(particlesCmd->tm, deltaT);

        auto updateInfo = particlesCmd->emittersPool.getAndResetUpdateInfo();
        DispatchParticlesSystemEvents(particlesCmd->refEntityId, updateInfo);
    }
}

void DrawParticlesExecutor::draw(RenderState& renderState, DrawCmdSlice& slice) {
    shader->bind();
    shader->setUniformMat4(UniformType::CameraMat, renderState.proj2dMat);

    const DrawParticlesCmd* prevCmd = nullptr;

    for(size_t i = slice.startIdx; i < slice.endIdx; ++i) {
        auto& cmd = *queue[i];
        renderState.startCommand(cmd);

        if(!prevCmd || !Math::IsEqual(prevCmd->alpha, cmd.alpha)) {
            shader->setUniform1f(UniformType::Alpha, cmd.alpha);
        }

        if(!prevCmd || prevCmd->texObj != cmd.texObj || prevCmd->renderConfig.textureInfo != cmd.renderConfig.textureInfo) {
            shader->setTexture2d(UniformType::Texture, 0, *cmd.texObj,
                cmd.renderConfig.textureInfo);   
        }

        for(auto& emitter : cmd.emittersPool.getEmitters()) {
            if(emitter->activeCount <= 0) {
                continue;
            };
            shader->setUniformMat4(UniformType::ModelMat, emitter->getTransformMat());
            {
                geom->setExtraVboData(&emitter->instanceData[0], emitter->instanceData.size() * sizeof(ParticleInstanceData));
                geom->bind();
                geom->drawTrianglesInstanced(emitter->activeCount);
                geom->unbind();
            }
        }

        prevCmd = &cmd;
    }

    shader->unbind();
}