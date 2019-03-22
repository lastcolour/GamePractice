#ifndef __RENDER_SIMPLE_LOGIC_HPP__
#define __RENDER_SIMPLE_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"

#include <memory>

class RenderMaterial;
class RenderGeometry;

class RenderSimpleLogic : public GameLogic,
    public ETNode<ETRenderEvents>,
    public ETNode<ETRenderSimpleLogic> {
public:

    RenderSimpleLogic();
    virtual ~RenderSimpleLogic();

    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETRenderEvents
    void ET_onRender(const RenderContext& renderCtx) override;
    void ET_onRenderPortResized() override { }

    // ETRenderSimpleLogic
    void ET_setRenderParams(const RenderLogicParams& params) override;
    void ET_getRenderParams(RenderLogicParams& params) override;

private:

    Mat4 getModelMat() const;

private:

    RenderLogicParams params;
    Vec2 scale;
    std::shared_ptr<RenderMaterial> mat;
    std::shared_ptr<RenderGeometry> geom;
};

#endif /* __RENDER_SIMPLE_LOGIC_HPP__ */