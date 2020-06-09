#ifndef __RENDER_TEXT_LOGIC_HPP__
#define __RENDER_TEXT_LOGIC_HPP__

#include "Render/Logics/RenderNode.hpp"
#include "Math/AABB.hpp"

class RenderGlyph;
class RenderGeometry;

class RenderTextLogic : public RenderNode,
    public ETNode<ETRenderTextLogic> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderTextLogic();
    virtual ~RenderTextLogic();

    // EntityLogic
    bool init() override;

    // ETRenderTextLogic
    void ET_setFont(const char* fontName) override;
    void ET_setColor(const ColorB& col) override;
    void ET_setText(const char* str) override;
    void ET_setFontSize(int fontSize) override;
    virtual AABB2D ET_getTextAABB() const override;

protected:

    // RenderNode
    void onRender(RenderContext& renderCtx) override;

private:

    void calcTextSize();

private:

    Vec2 textSize;
    std::shared_ptr<RenderFont> font;
    std::string text;
    ColorB color;
    float fontScale;
};

#endif /* __RENDER_TEXT_LOGIC_HPP__ */