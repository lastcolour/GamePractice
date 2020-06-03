#ifndef __UI_BACKGROUND_HPP__
#define __UI_BACKGROUND_HPP__

#include "Entity/EntityLogic.hpp"
#include "Render/ETRenderInterfaces.hpp"

class UIBackground : public EntityLogic,
    public ETNode<ETRenderEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIBackground();
    virtual ~UIBackground();

    // EntityLogic
    bool init() override;
    void deinit() override;

    void ET_onRender(const RenderContext& renderCtx) override {}
    void ET_onRenderPortResized() override;
};

#endif /* __UI_BACKGROUND_HPP__ */