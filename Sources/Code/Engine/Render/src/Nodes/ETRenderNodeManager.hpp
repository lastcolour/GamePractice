#ifndef __ET_RENDER_NODE_MANAGER_HPP__
#define __ET_RENDER_NODE_MANAGER_HPP__

#include "Core/Core.hpp"
#include "Math/Transform.hpp"

class Node;
class ImageBuffer;
enum class DrawContentFilter;

enum class RenderNodeType {
    Simple = 0,
    Image,
    ColoredTexture,
    Gradient,
    Text,
    ParticleEmmiter,
    Blur,
    NinePatchImage
};

struct RenderNodeCreateParams {
    Transform tm;
    float alpha;
    int drawPriority;
    RenderNodeType type;
};

struct ETRenderNodeManager {
    virtual ~ETRenderNodeManager() = default;
    virtual Node* ET_createNode(RenderNodeType nodeType) = 0;
    virtual void ET_removeNode(Node* node) = 0;
    virtual void ET_initRenderNode(Node* node) = 0;
    virtual void ET_drawFrame() = 0;
    virtual void ET_drawFrameToBuffer(ImageBuffer& imageBuffer, DrawContentFilter filter) = 0;
};

struct ETRenderProxyNodeEvents {
    virtual ~ETRenderProxyNodeEvents() = default;
    virtual void ET_syncWithRender() = 0;
};

struct ETParticlesUpdate {
    virtual ~ETParticlesUpdate() = default;
    virtual void ET_updateEmitter(float dt) = 0;
};

#endif /* __ET_RENDER_NODE_MANAGER_HPP__ */