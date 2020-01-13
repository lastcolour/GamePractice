#ifndef __RENDER_NODE_HPP__
#define __RENDER_NODE_HPP__

#include "Render/ETRenderInterfaces.hpp"
#include "Entity/EntityLogic.hpp"

class RenderNode : public EntityLogic,
    public ETNode<ETRenderNode>,
    public ETNode<ETRenderEvents> {
public:

    RenderNode();
    virtual ~RenderNode();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETRenderNode
    bool ET_isVisible() const override;
    void ET_hide() override;
    void ET_show() override;
    void ET_setDrawPriority(int newDrawPriority) override;
    int ET_getDrawPriority() const override;

private:

    int drawPriority;
    bool isVisible;
};

#endif /* __RENDER_NODE_HPP__ */