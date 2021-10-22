#ifndef __RENDER_LIGHTING_BOLT_EFFECT_HPP__
#define __RENDER_LIGHTING_BOLT_EFFECT_HPP__

#include "Logics/RenderNode.hpp"

class RenderLightingBoltEffect : public RenderNode {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderLightingBoltEffect();
    virtual ~RenderLightingBoltEffect();

    // RenderNode
    void init() override;
    void deinit() override;

private:
};

#endif /* __RENDER_LIGHTING_BOLT_EFFECT_HPP__ */