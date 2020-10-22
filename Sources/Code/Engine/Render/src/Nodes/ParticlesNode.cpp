#include "Nodes/ParticlesNode.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Render/ETRenderManager.hpp"

ParticlesNode::ParticlesNode() {
}

ParticlesNode::~ParticlesNode() {
}

void ParticlesNode::update(float dt) {
    auto scale = Render::CalcGeomScaleForSize(Vec2i(30, 30), *geom);

    for(int i = 0; i < 100; ++i) {
        auto& p = particles[i];
        p.lifetime -= dt;
        if(p.lifetime < 0.f) {
            p.tm.pt = tm.pt + Vec3(300.f, 300.f, 0.f);
            p.lifetime = 1.f;
            p.velocity = Vec3( static_cast<float>((i % 3) * (1.f - 2.f * (i % 3))), static_cast<float>(i % 5) * (1.f - 2.f * (i % 2)), 0.f);
            p.velocity = p.velocity.getNormilized();
            p.velocity *= 300.f;
            //p.acceleration = Vec3(p.velocity.z, p.velocity.y, p.velocity.x);
        }
        p.velocity += p.acceleration * dt;
        p.tm.pt += p.velocity * dt;

        instacesData[i].mat = Render::CalcModelMat(p.tm, Vec3(scale, 1.f), *geom);
    }
}

void ParticlesNode::onInit() {
    for(int i = 0; i < 100; ++i) {
        Particle p;
        p.lifetime = 1.f - (i % 13) / 13.f;
        p.tm = tm;
        p.tm.pt += Vec3(300.f, 300.f, 0.f);
        p.velocity = Vec3( static_cast<float>((i % 3) * (1.f - 2.f * (i % 3))), static_cast<float>(i % 5) * (1.f - 2.f * (i % 2)), 0.f);
        p.velocity = p.velocity.getNormilized();
        p.velocity *= 300.f;
        p.acceleration = Vec3(0.f);
        particles.push_back(p);
        instacesData.push_back({ColorF(1.f), Math::Matrix4(1.f)});
    }

    setBlendingMode(RenderBlendingType::ONE_MINUS_SRC_MINUS_ALPHA);
    setGeometry(PrimitiveGeometryType::Particles);
    setMaterial("particle");
    setSetupAlpha(false);
    ET_SendEventReturn(tex, &ETRenderTextureManger::ET_createTexture, "Images/block.png", ETextureType::RGBA);
}

void ParticlesNode::onRender(RenderContext& ctx) {
    update(0.01f);

    mat->setTexture2D("tex", tex->texId);
    geom->drawInstanced(&instacesData[0], 100);
}