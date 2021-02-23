#ifndef __PARTICLES_NODE_HPP__
#define __PARTICLES_NODE_HPP__

#include "Nodes/Node.hpp"
#include "RenderTexture.hpp"
#include "Particles/ParticleUpdate.hpp"

class ParticlesNode : public Node {
public:

    ParticlesNode();
    virtual ~ParticlesNode();

    void setConfig(const ParticleEmitterRenderConfig& newRenderConf);

    EmitterParticles& getParticles();

    // Node
    bool canRender() const override;

protected:

    // Node
    void onInit() override;
    void onRender(RenderContext& ctx) override;
    Mat4 calcModelMat(const Transform& newTm) override;

private:

    std::shared_ptr<RenderTexture> tex;
    ParticleEmitterRenderConfig renderConfig;
    EmitterParticles particles;
};

#endif /* __PARTICLES_NODE_HPP__ */