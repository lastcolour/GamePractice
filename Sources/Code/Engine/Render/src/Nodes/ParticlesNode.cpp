#include "Nodes/ParticlesNode.hpp"
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

    BlendMode mode;
    switch(renderConfig.blending) {
        case BlendingConfig::Normal: {
            mode.src = BlendType::SRC_ALPHA;
            mode.dst = BlendType::ONE_MINUS_SRC_ALPHA;
            break;
        }
        case BlendingConfig::Additive: {
            mode.src = BlendType::SRC_ALPHA;
            mode.dst = BlendType::ONE;
            break;
        }
        default: {
            assert(false && "Invalid blend mode");
        }
    }
    setBlendingMode(mode);
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
        geom->drawInstanced(&emitter->instaceData[0], emitter->activeCount);
    }
}