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
    void ET_setText(const std::string& str) override;

private:

    Mat4 getModelMat() const;
    void createVAO();
    void calcTextAABB();
    Vec2 initVertexForGlyph(const Vec2& pt, const RenderGlyph& glyph);

private:

    unsigned int vaoId;
    unsigned int vboId;
    std::shared_ptr<RenderMaterial> mat;
    std::shared_ptr<RenderFont> font;
    AABB2D aabb;
    std::string text;
};

#endif /* __RENDER_TEXT_LOGIC_HPP__ */