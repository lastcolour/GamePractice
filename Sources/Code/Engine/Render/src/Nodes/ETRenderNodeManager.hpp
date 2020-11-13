#ifndef __ET_RENDER_NODE_MANAGER_HPP__
#define __ET_RENDER_NODE_MANAGER_HPP__

#include "Core/Core.hpp"
#include "Math/Transform.hpp"

class Node;

enum class RenderNodeType {
    Simple = 0,
    Image,
    ColoredTexture,
    Gradient,
    Text,
    ParticleEmmiter
};

struct RenderNodeCreateParams {
    Transform tm;
    float alpha;
    int drawPriority;
    RenderNodeType type;
};

struct ETRenderNodeManager {
    virtual ~ETRenderNodeManager() = default;
    virtual Node* ET_createNode(const RenderNodeCreateParams& params) = 0;
    virtual void ET_removeNode(Node* node) = 0;
    virtual void ET_initRenderNode(Node* node) = 0;
    virtual void ET_update() = 0;
    virtual void ET_updateParticles(float dt) = 0;
};

struct ETRenderProxyNodeEvents {
    virtual ~ETRenderProxyNodeEvents() = default;
    virtual void ET_syncWithRender() = 0;
};

#endif /* __ET_RENDER_NODE_MANAGER_HPP__ */