#ifndef __PARTICLES_NODE_HPP__
#define __PARTICLES_NODE_HPP__

#include "Nodes/Node.hpp"
#include "RenderTexture.hpp"
#include "Particles/ParticleUpdate.hpp"

class ParticlesNode : public Node,
    public ETNode<ETParticleEmitterProxyNode> {
public:

    ParticlesNode();
    virtual ~ParticlesNode();

    void ET_update(float dt) override;
    void ET_setConfig(const ParticleEmitterEmissionConfig& emissionConf,
        const ParticleEmitterMovementConfig& movementConf,
        const ParticleEmitterColorConfig& colorConf,
        const ParticleEmitterRenderConfig& renderConf) override;

protected:

    void onInitConnections() override;
    void onInitRender() override;
    void onRender(RenderContext& ctx) override;
    bool isVisible() const override;

private:

    std::shared_ptr<RenderTexture> tex;
    ParticleEmitterRenderConfig renderConfig;
    EmitterState state;
};

#endif /* __PARTICLES_NODE_HPP__ */