#ifndef __RENDER_TEXT_LOGIC_HPP__
#define __RENDER_TEXT_LOGIC_HPP__

#include "Logics/RenderNode.hpp"

class RenderFont;
class ReflectContext;

class RenderTextLogic : public RenderNode,
    public ETNode<ETRenderTextLogic> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderTextLogic();
    virtual ~RenderTextLogic();

    // EntityLogic
    void init() override;

    // ETRenderTextLogic
    void ET_setColor(const ColorB& newColor) override;
    void ET_setText(const char* str) override;
    void ET_setFontHeight(int fontHeight) override;
    AABB2D ET_getTextAABB() const override;
    AABB2Di ET_getTextAABBi() const override;

protected:

    void onSyncWithRender() override;

private:

    void calcTextSize();

private:

    std::shared_ptr<RenderFont> font;
    int fontHeight;
    std::string text;
    ColorB color;
    bool isTextChanged;
    bool isColorChanged;
};

#endif /* __RENDER_TEXT_LOGIC_HPP__ */