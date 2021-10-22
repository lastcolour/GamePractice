#ifndef __LIGHTING_BOLT_EFFECT_NODE_HPP__
#define __LIGHTING_BOLT_EFFECT_NODE_HPP__

#include "Nodes/Node.hpp"

class LightingBoltEffectNode : public Node {
public:

    LightingBoltEffectNode();
    virtual ~LightingBoltEffectNode();

protected:

    // Node
    void onInit() override;
    void onRender(RenderContext& ctx) override;
};

#endif /* __LIGHTING_BOLT_EFFECT_NODE_HPP__ */