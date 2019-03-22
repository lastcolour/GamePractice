#ifndef __RENDER_TEXT_LOGIC_HPP__
#define __RENDER_TEXT_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "Render/ETRenderInterfaces.hpp"

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

    std::shared_ptr<RenderMaterial> mat;
    std::shared_ptr<RenderFont> font;
    std::string text;
};

#endif /* __RENDER_TEXT_LOGIC_HPP__ */