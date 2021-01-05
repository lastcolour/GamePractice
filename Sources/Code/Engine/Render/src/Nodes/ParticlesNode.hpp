#ifndef __PARTICLES_NODE_HPP__
#define __PARTICLES_NODE_HPP__

#include "Nodes/Node.hpp"
#include "RenderTexture.hpp"
#include "Particles/ParticleUpdate.hpp"

#include <atomic>

class ParticlesNode : public Node {
public:

    ParticlesNode();
    virtual ~ParticlesNode();

    void update(float dt);

    void setConfig(const ParticleEmitterEmissionConfig& emissionConf,
        const ParticleEmitterMovementConfig& movementConf,
        const ParticleEmitterColorConfig& colorConf,
        const ParticleEmitterRenderConfig& renderConf);

protected:

    // Node
    void onInit() override;
    void onRender(RenderContext& ctx) override;
    bool isVisible() const override;

private:

    std::shared_ptr<RenderTexture> tex;
    ParticleEmitterRenderConfig renderConfig;
    EmitterState state;
    std::atomic<bool> needUpdate;
};

#endif /* __PARTICLES_NODE_HPP__ */