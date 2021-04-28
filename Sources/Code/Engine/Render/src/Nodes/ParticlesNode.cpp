#include "Nodes/ParticlesNode.hpp"
#include "Math/MatrixTransform.hpp"
#include "RenderUtils.hpp"

#include <cassert>

ParticlesNode::ParticlesNode() {
}

ParticlesNode::~ParticlesNode() {
}

void ParticlesNode::setConfig(const ParticlesEmitterRenderConfig& newRenderConf) {
    renderConfig = newRenderConf;

    texObj = RenderUtils::CreateTexture(renderConfig.textureInfo, ETextureDataType::RGBA);

    bool preMultipliedAlpha = true;
    auto mode = RenderUtils::GetBlendMode(renderConfig.blending, preMultipliedAlpha);
    setBlendingMode(mode);
}

ParticlesEmittersPool& ParticlesNode::getEmittersPool() {
    return emittersPool;
}

bool ParticlesNode::canRender() const {
    if(!emittersPool.hasAlive()) {
        return false;
    }
    if(!texObj) {
        return false;
    }
    return Node::canRender();
}

void ParticlesNode::onInit() {
    bool preMultipliedAlpha = true;
    auto mode = RenderUtils::GetBlendMode(renderConfig.blending, preMultipliedAlpha);
    setBlendingMode(mode);
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
    RenderUtils::ApplyTextureInfo(*texObj, renderConfig.textureInfo);
    shader->setTexture2d(UniformType::Texture, 0, *texObj);
    for(auto& emitter : emittersPool.getEmitters()) {
        if(emitter->activeCount <= 0) {
            continue;
        }
        Mat4 resMat = emitter->getTransformMat();
        shader->setUniformMat4(UniformType::ModelMat, resMat);
        geom->drawInstanced(&emitter->instaceData[0], emitter->activeCount);
    }
}