#ifndef __RENDER_TEXT_LOGIC_HPP__
#define __RENDER_TEXT_LOGIC_HPP__

#include "Logics/RenderNode.hpp"

class RenderFont;

class RenderTextLogic : public RenderNode,
    public ETNode<ETRenderTextLogic> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderTextLogic();
    virtual ~RenderTextLogic();

    // ETRenderTextLogic
    void ET_setColor(const ColorB& newColor) override;
    void ET_setText(const char* str) override;
    void ET_setFontHeight(float newFontHeight) override;
    void ET_setFontType(EFontType newFontType) override;
    EFontType ET_getFontType() const override;
    AABB2D ET_getTextAABB() const override;

protected:

    // RenderNode
    void onInit() override;

private:

    void calcTextSize();

private:

    std::shared_ptr<RenderFont> font;
    std::string text;
    ColorB color;
    float fontHeight;
    EFontType fontType;
};

#endif /* __RENDER_TEXT_LOGIC_HPP__ */