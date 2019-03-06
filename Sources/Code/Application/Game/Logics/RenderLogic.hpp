#ifndef __RENDER_LOGIC_HPP__
#define __RENDER_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "Render/Color.hpp"
#include "Render/ETRenderInterfaces.hpp"

#include <memory>

class RenderMaterial;
class RenderGeometry;

class RenderLogic : public GameLogic,
    public ETNode<ETRenderEvents>,
    public ETNode<ETRenderLogic> {
public:

    RenderLogic();
    virtual ~RenderLogic();

    bool init(const JSONNode& node) override;

    // ETRenderEvents
    void ET_onRender(const RenderContext& renderCtx) override;

    // ETRenderLogic
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

#endif /* __RENDER_LOGIC_HPP__ */