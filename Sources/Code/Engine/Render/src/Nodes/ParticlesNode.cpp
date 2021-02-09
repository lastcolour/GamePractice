#include "Nodes/ParticlesNode.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Render/ETRenderManager.hpp"
#include "Math/MatrixTransform.hpp"

#include <cassert>

ParticlesNode::ParticlesNode() {
}

ParticlesNode::~ParticlesNode() {
}

void ParticlesNode::update(float dt) {
    if(!needUpdate.load()) {
        return;
    }
    needUpdate.store(false);
    state.update(tm, dt);
}

void ParticlesNode::setConfig(const ParticleEmitterEmissionConfig& emissionConf, const ParticleEmitterMovementConfig& movementConf,
    const ParticleEmitterColorConfig& colorConf, const ParticleEmitterRenderConfig& renderConf, const ParticleEmitterGravityFields& gravityConf) {

    renderConfig = renderConf;
    state.emissionConfig = emissionConf;
    state.movementConifg = movementConf;
    state.colorConfig = colorConf;
    state.gravityConfig = gravityConf;

    state.reset();
    ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createFromImage, renderConf.texture.c_str(), ETextureType::RGBA);

    needUpdate.store(true);

    setModelMatDirty();
}

bool ParticlesNode::isVisible() const {
    if(state.lifeType == EmitterLifeType::Finished && !state.emissionConfig.loop) {
        return false;
    }
    if(!tex) {
        return false;
    }
    return Node::isVisible();
}

void ParticlesNode::onInit() {
    setBlendingMode(BlendMode{BlendType::SRC_ALPHA, BlendType::ONE_MINUS_SRC_ALPHA});
    setGeometry(PrimitiveGeometryType::Particles);
    setShader("particle");
}

Mat4 ParticlesNode::calcModelMat(const Transform& newTm) {
    tm = newTm;

    Mat4 resMat = Mat4(1.f);

    if(state.emissionConfig.emitterSpace == EmitterSpace::Local) {
        Math::AddTranslate(resMat, newTm.pt);
        Math::AddRotate(resMat, newTm.quat);
        Math::AddScale(resMat, newTm.scale);
    }

    return resMat;
}

void ParticlesNode::onRender(RenderContext& ctx) {
    needUpdate.store(true);

    shader->setTexture2D(UniformType::Texture, *tex);
    geom->drawInstanced(&state.instaceData[0], state.activeCount);
}