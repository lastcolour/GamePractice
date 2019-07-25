#ifndef __RENDER_TEXT_LOGIC_HPP__
#define __RENDER_TEXT_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Math/AABB.hpp"

class RenderGlyph;

class RenderTextLogic : public GameLogic,
    public ETNode<ETRenderEvents>,
    public ETNode<ETRenderTextLogic> {
public:

    RenderTextLogic();
    virtual ~RenderTextLogic();

    // GameLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETRenderEvents
    void ET_onRender(const RenderContext& renderCtx) override;
    void ET_onRenderPortResized() override {}

    // ETRenderTestLogic
    void ET_setMaterial(const char* matName) override;
    void ET_setColor(const ColorB& col) override;
    void ET_setText(const char* str) override;
    void ET_setFontSize(float fontSize) override;
    virtual const AABB2D& ET_getTextAABB() const override;

private:

    void createVAO();
    void calcTextAABB();

private:

    AABB2D aabb;
    std::shared_ptr<RenderMaterial> mat;
    std::shared_ptr<RenderFont> font;
    std::string text;
    float fontSize;
    unsigned int vaoId;
    unsigned int vboId;
    ColorB color;
};

#endif /* __RENDER_TEXT_LOGIC_HPP__ */