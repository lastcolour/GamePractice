#include "Nodes/ParticlesNode.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Render/ETRenderManager.hpp"
#include "Math/MatrixTransform.hpp"

#include <cassert>

ParticlesNode::ParticlesNode() {
}

ParticlesNode::~ParticlesNode() {
}

void ParticlesNode::setConfig(const ParticlesEmitterRenderConfig& newRenderConf) {
    renderConfig = newRenderConf;
    ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createFromImage,
        renderConfig.texture.c_str(), ETextureType::RGBA);
    setModelMatDirty();
}

ParticlesEmittersPool& ParticlesNode::getEmittersPool() {
    return emittersPool;
}

bool ParticlesNode::canRender() const {
    if(!emittersPool.hasAlive()) {
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
    return Mat4(1.f);
}

const Transform& ParticlesNode::getTransform() const {
    return tm;
}

void ParticlesNode::onRender(RenderContext& ctx) {
    shader->setTexture2D(UniformType::Texture, *tex);
    for(auto& emitter : emittersPool.getEmitters()) {
        if(emitter->activeCount <= 0) {
            continue;
        }
        Mat4 resMat = emitter->getTransformMat();
        shader->setUniformMat4(UniformType::ModelMat, resMat);

        auto& particles = emitter->particles;
        geom->drawInstanced(&emitter->instaceData[0], emitter->activeCount);
    }
}