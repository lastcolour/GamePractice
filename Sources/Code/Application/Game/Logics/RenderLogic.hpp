#ifndef __RENDER_LOGIC_HPP__
#define __RENDER_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Game/GameETInterfaces.hpp"

#include <memory>

class RenderMaterial;
class RenderGeometry;

class RenderLogic : public GameLogic,
    public ETNode<ETRenderEvents>,
    public ETNode<ETRenderLogic> {
public:

    RenderLogic();
    virtual ~RenderLogic();

    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETRenderEvents
    void ET_onRender(const RenderContext& renderCtx) override;
    void ET_onRenderPortResize(const Vec2i& size) override {}

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