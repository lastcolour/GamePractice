#ifndef __ET_RENDER_NODE_MANAGER_HPP__
#define __ET_RENDER_NODE_MANAGER_HPP__

class Node;
class ImageBuffer;
enum class DrawContentFilter;

struct ETRenderNodeManager {
    virtual ~ETRenderNodeManager() = default;
    virtual void ET_addUpdateEvent(std::function<void(void)> func) = 0;
    virtual void ET_removeNode(Node* node) = 0;
    virtual void ET_initRenderNode(Node* node) = 0;
    virtual void ET_drawFrame() = 0;
    virtual void ET_drawFrameToBuffer(void* outBuffer, DrawContentFilter filter) = 0;
};

struct ETParticlesUpdate {
    virtual ~ETParticlesUpdate() = default;
    virtual void ET_updateEmitter(float dt) = 0;
};

#endif /* __ET_RENDER_NODE_MANAGER_HPP__ */