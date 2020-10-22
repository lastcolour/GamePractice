#ifndef __PARTICLES_NODE_HPP__
#define __PARTICLES_NODE_HPP__

#include "Nodes/Node.hpp"
#include "RenderTexture.hpp"

struct Particle {
    float lifetime;
    Vec3 velocity;
    Vec3 acceleration;
    Transform tm;
};

struct InstaceData {
    ColorF color;
    Math::Matrix4 mat;
};

class ParticlesNode : public Node {
public:

    ParticlesNode();
    virtual ~ParticlesNode();

    void update(float dt);

protected:

    void onInit() override;
    void onRender(RenderContext& ctx) override;

private:

    std::shared_ptr<RenderTexture> tex;
    std::vector<Particle> particles;
    std::vector<InstaceData> instacesData;
};

#endif /* __PARTICLES_NODE_HPP__ */