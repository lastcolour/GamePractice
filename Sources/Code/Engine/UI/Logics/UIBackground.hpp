#ifndef __UI_BACKGROUND_HPP__
#define __UI_BACKGROUND_HPP__

#include "Entity/EntityLogic.hpp"
#include "Render/ETRenderInterfaces.hpp"

class UIBackground : public EntityLogic,
    public ETNode<ETRenderEvents> {
public:

    UIBackground();
    virtual ~UIBackground();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    void ET_onRender(const RenderContext& renderCtx) override {}
    void ET_onRenderPortResized() override;
};

#endif /* __UI_BACKGROUND_HPP__ */