#ifndef __DRAW_COMMAND_PROXY_HPP__
#define __DRAW_COMMAND_PROXY_HPP__

#include "Entity/EntityLogic.hpp"
#include "Render/ETRenderNode.hpp"
#include "Commands/ETDrawCommands.hpp"

class DrawCommandProxy : public EntityLogic,
    public ETNode<ETRenderNode>,
    public ETNode<ETEntityEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    DrawCommandProxy(EDrawCmdType cmdType);
    virtual ~DrawCommandProxy() = 0;

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETRenderNode
    bool ET_isVisible() const override;
    void ET_hide() override;
    void ET_show() override;
    void ET_setZIndex(int newZIndex) override;
    int ET_getZIndex() const override;
    void ET_setAlphaMultiplier(float newAlphaMult) override;
    float ET_getAlphaMultiplier() const override;
    void ET_setStencilData(const StencilWirteReadData& newSteniclData) override;
    void ET_setNormalizationScale(float newNormScale) override;
    float ET_getNormalizationScale() const override;
    bool ET_setEmitEvents(bool flag) override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override;
    void ET_onLoaded() override;

protected:

    virtual void onInit() = 0;

protected:

    DrawCmd* cmd;
    StencilWirteReadData stencilData;
    float alpha;
    float alphaMult;
    float normScale;
    int zIndex;
    EDrawCmdType type;
    bool isVisible;
    bool isLoaded;
    bool emitEvents;
};

#endif /* __DRAW_COMMAND_PROXY_HPP__ */