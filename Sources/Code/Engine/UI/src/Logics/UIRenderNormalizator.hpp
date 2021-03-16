#ifndef __UI_RENDER_NORMALIZATOR_HPP__
#define __UI_RENDER_NORMALIZATOR_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUIViewPort.hpp"

class UIRenderNormalizator : public EntityLogic,
    public ETNode<ETEntityEvents>,
    public ETNode<ETUIViewPortEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIRenderNormalizator();
    virtual ~UIRenderNormalizator();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override {}
    void ET_onLoaded() override;

    // ETUIViewPortEvents
    void ET_onViewPortChanged(const Vec2i& newSize) override;

private:

    float factor;
};

#endif /* __UI_RENDER_NORMALIZATOR_HPP__ */