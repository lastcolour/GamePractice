#include "Nodes/ParticlesNode.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Render/ETRenderManager.hpp"
#include "Core/ETLogger.hpp"

ParticlesNode::ParticlesNode() {
}

ParticlesNode::~ParticlesNode() {
}

void ParticlesNode::onInitConnections() {
    ETNode<ETParticleEmitterProxyNode>::connect(nodeId);
}

void ParticlesNode::ET_update(float dt) {
    state.update(tm, dt);
}

void ParticlesNode::ET_setConfig(const ParticleEmitterEmissionConfig& emissionConf, const ParticleEmitterMovementConfig& movementConf,
    const ParticleEmitterColorConfig& colorConf, const ParticleEmitterRenderConfig& renderConf) {

    renderConfig = renderConf;
    state.emissionConfig = emissionConf;
    state.movementConifg = movementConf;
    state.colorConfig = colorConf;

    state.reset();
    ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createTexture, renderConf.texture.c_str(), ETextureType::RGBA);
}

bool ParticlesNode::isVisible() const {
    if(state.activeCount == 0) {
        return false;
    }
    if(!tex) {
        return false;
    }
    return Node::isVisible();
}

void ParticlesNode::onInitRender() {
    setBlendingMode(RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA);
    setGeometry(PrimitiveGeometryType::Particles);
    setMaterial("particle");
    setSetupAlpha(false);
}

void ParticlesNode::onRender(RenderContext& ctx) {
    mat->setTexture2D("tex", tex->texId);
    geom->drawInstanced(&state.instaceData[0], state.activeCount);
}