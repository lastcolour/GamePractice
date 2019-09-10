#ifndef __RENDER_TEXT_LOGIC_HPP__
#define __RENDER_TEXT_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Math/AABB.hpp"

class RenderGlyph;
class RenderGeometry;

class RenderTextLogic : public EntityLogic,
    public ETNode<ETRenderEvents>,
    public ETNode<ETRenderTextLogic> {
public:

    RenderTextLogic();
    virtual ~RenderTextLogic();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETRenderEvents
    void ET_onRender(const RenderContext& renderCtx) override;
    void ET_onRenderPortResized() override {}

    // ETRenderTextLogic
    void ET_setMaterial(const char* matName) override;
    void ET_setColor(const ColorB& col) override;
    void ET_setText(const char* str) override;
    void ET_setFontSize(int fontSize) override;
    virtual AABB2D ET_getTextAABB() const override;

private:

    void calcTextSize();

private:

    Vec2 textSize;
    std::shared_ptr<RenderMaterial> mat;
    std::shared_ptr<RenderFont> font;
    std::shared_ptr<RenderGeometry> geom;
    std::string text;
    ColorB color;
    float fontScale;
};

#endif /* __RENDER_TEXT_LOGIC_HPP__ */