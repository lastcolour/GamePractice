#ifndef __RENDER_STENCIL_LOGIC_HPP__
#define __RENDER_STENCIL_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Render/RenderCommon.hpp"

class RenderStencilLogic : public EntityLogic,
    public ETNode<ETEntityEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderStencilLogic();
    virtual ~RenderStencilLogic();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override {}
    void ET_onLoaded() override;

private:

    int refVal;
    EStencilOpType mode;
};

#endif /* __RENDER_STENCIL_LOGIC_HPP__ */