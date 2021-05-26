#ifndef __RENDER_QUAD_LOGIC_HPP__
#define __RENDER_QUAD_LOGIC_HPP__

#include "Logics/RenderNode.hpp"
#include "Shaders/RenderShadersParams.hpp"

class RenderQuadLogic : public RenderNode {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderQuadLogic();
    virtual ~RenderQuadLogic();

protected:

    // RenderNode
    void onInit() override;

private:

    Reflect::PolymorphPtr<RenderShaderParams> shaderParams;
    Vec2 size;
};

#endif /* __RENDER_QUAD_LOGIC_HPP__  */