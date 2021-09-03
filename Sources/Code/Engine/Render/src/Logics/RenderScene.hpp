#ifndef __RENDER_SCENE_HPP__
#define __RENDER_SCENE_HPP__

#include "Entity/EntityLogic.hpp"
#include "Render/ETRenderNode.hpp"

class RenderScene : public EntityLogic,
    public ETNode<ETEntityEvents>,
    public ETNode<ETRenderNodeEvents>,
    public ETNode<ETRenderScene> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    RenderScene();
    virtual ~RenderScene();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override;
    void ET_onLoaded() override;

    // ETRenderScene
    void ET_addItem(int layer, EntityId entId) override;
    void ET_removeItem(EntityId entId) override;

    // ETRenderNodeEvents
    void ET_onHidden(bool flag) override;
    void ET_onZIndexChanged(int newZIndex) override;
    void ET_onAlphaMultChanged(float newAlphaMult) override;
    void ET_onNormScaleChanged(float newNormScale) override;

private:

    enum class EChildStencilPolicy {
        ReadIncrease = 0,
        Read,
        None
    };

    struct ChildNode {
        EntityId entId;
        int depth;
    };

private:

    std::vector<ChildNode> children;
    int zIndexStep;
    EChildStencilPolicy stencilPolicy;
    bool buildChildListOnLoad;
};

#endif /* __RENDER_SCENE_HPP__ */