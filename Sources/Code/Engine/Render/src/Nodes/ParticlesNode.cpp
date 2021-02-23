#include "Nodes/ParticlesNode.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Render/ETRenderManager.hpp"
#include "Math/MatrixTransform.hpp"

#include <cassert>

ParticlesNode::ParticlesNode() {
}

ParticlesNode::~ParticlesNode() {
}

void ParticlesNode::setConfig(const ParticleEmitterRenderConfig& newRenderConf) {
    renderConfig = newRenderConf;
    ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createFromImage,
        renderConfig.texture.c_str(), ETextureType::RGBA);
    setModelMatDirty();
}

EmitterParticles& ParticlesNode::getParticles() {
    return particles;
}

bool ParticlesNode::canRender() const {
    if(!particles.hasAlive()) {
        return false;
    }
    if(!tex) {
        return false;
    }
    return Node::canRender();
}

void ParticlesNode::onInit() {
    setBlendingMode(BlendMode{BlendType::SRC_ALPHA, BlendType::ONE_MINUS_SRC_ALPHA});
    setGeometry(PrimitiveGeometryType::Particles);
    setShader("particle");
}

Mat4 ParticlesNode::calcModelMat(const Transform& newTm) {
    Mat4 resMat = Mat4(1.f);

    if(particles.emissionConfig.emitterSpace == EmitterSpace::Local) {
        Math::AddTranslate(resMat, newTm.pt);
        Math::AddRotate(resMat, newTm.quat);
        Math::AddScale(resMat, newTm.scale);
    }

    return resMat;
}

void ParticlesNode::onRender(RenderContext& ctx) {
    shader->setTexture2D(UniformType::Texture, *tex);
    shader->setUniformMat4(UniformType::ModelMat, modelMat);
    geom->drawInstanced(&particles.instaceData[0], particles.activeCount);
}