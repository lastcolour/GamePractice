#ifndef __RENDER_SIMPLE_LOGIC_HPP__
#define __RENDER_SIMPLE_LOGIC_HPP__

#include "Render/Logics/RenderNode.hpp"

#include <memory>

class RenderMaterial;
class RenderGeometry;

class RenderSimpleLogic : public RenderNode,
    public ETNode<ETRenderSimpleLogic> {
public:

    RenderSimpleLogic();
    virtual ~RenderSimpleLogic();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETRenderEvents
    void ET_onRender(const RenderContext& renderCtx) override;
    void ET_onRenderPortResized() override { }

    // ETRenderSimpleLogic
    void ET_setColor(const ColorB& col) override;
    void ET_setSize(const Vec2& size) override;
    void ET_setMaterial(const char* matName) override;
    void ET_setGeometry(const char* geomName) override;

private:

    Mat4 getModelMat() const;

private:

    Vec2 scale;
    ColorB color;
    std::shared_ptr<RenderMaterial> mat;
    std::shared_ptr<RenderGeometry> geom;
};

#endif /* __RENDER_SIMPLE_LOGIC_HPP__ */