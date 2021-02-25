#ifndef __PARTICLES_NODE_HPP__
#define __PARTICLES_NODE_HPP__

#include "Nodes/Node.hpp"
#include "RenderTexture.hpp"
#include "Particles/ParticlesEmittersPool.hpp"

class ParticlesNode : public Node {
public:

    ParticlesNode();
    virtual ~ParticlesNode();

    void setConfig(const ParticlesEmitterRenderConfig& newRenderConf);

    ParticlesEmittersPool& getEmittersPool();
    const Transform& getTransform() const;

    // Node
    bool canRender() const override;

protected:

    // Node
    void onInit() override;
    void onRender(RenderContext& ctx) override;
    Mat4 calcModelMat(const Transform& newTm) override;

private:

    std::shared_ptr<RenderTexture> tex;
    ParticlesEmitterRenderConfig renderConfig;
    ParticlesEmittersPool emittersPool;
};

#endif /* __PARTICLES_NODE_HPP__ */