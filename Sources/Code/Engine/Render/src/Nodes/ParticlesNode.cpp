#include "Nodes/ParticlesNode.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Render/ETRenderManager.hpp"
#include "Math/MatrixTransform.hpp"

ParticlesNode::ParticlesNode() {
}

ParticlesNode::~ParticlesNode() {
}

void ParticlesNode::update(float dt) {
    state.update(tm, dt);
}

void ParticlesNode::setConfig(const ParticleEmitterEmissionConfig& emissionConf, const ParticleEmitterMovementConfig& movementConf,
    const ParticleEmitterColorConfig& colorConf, const ParticleEmitterRenderConfig& renderConf) {

    renderConfig = renderConf;
    state.emissionConfig = emissionConf;
    state.movementConifg = movementConf;
    state.colorConfig = colorConf;

    state.reset();
    ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createFromImage, renderConf.texture.c_str(), ETextureType::RGBA);
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

void ParticlesNode::onInit() {
    setBlendingMode(RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA);
    setGeometry(PrimitiveGeometryType::Particles);
    setShader("particle");
}

void ParticlesNode::onRender(RenderContext& ctx) {
    Mat4 resMat = Mat4(1.f);

    if(state.emissionConfig.emitterSpace == EmitterSpace::Local) {
        Math::AddTranslate(resMat, tm.pt);
        Math::AddRotate(resMat, tm.quat);
        Math::AddScale(resMat, tm.scale);
    }

    shader->setTexture2D(UniformType::Texture, *tex);
    shader->setUniformMat4(UniformType::ModelMat, resMat);
    geom->drawInstanced(&state.instaceData[0], state.activeCount);
}