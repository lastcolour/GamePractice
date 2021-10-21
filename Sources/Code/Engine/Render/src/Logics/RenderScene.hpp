#ifndef __RENDER_SCENE_HPP__
#define __RENDER_SCENE_HPP__

#include "Entity/EntityLogic.hpp"
#include "Render/ETRenderNode.hpp"
#include "Render/ETRenderScene.hpp"

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
    void ET_onTransformChanged(const Transform& newTm) override {}
    void ET_onLoaded() override;

    // ETRenderScene
    void ET_addItem(int zIndexOffset, EntityId entId) override;
    void ET_removeItem(EntityId entId) override;
    const RenderSceneParams& ET_getParams() const override;
    void ET_setParams(RenderSceneParams& newParams) override;
    size_t ET_getItemsCount() const override;

    // ETRenderNodeEvents
    void ET_onHidden(bool flag) override;
    void ET_onZIndexChanged(int newZIndex) override;
    void ET_onAlphaMultChanged(float newAlphaMult) override;
    void ET_onNormScaleChanged(float newNormScale) override;

private:

    struct ChildNode {
    public:

        static void Reflect(ReflectContext& ctx);

    public:

        EntityId entId;
        int zIndexOffset{1};
        bool prevVisible{false};
    };

private:

    std::vector<ChildNode> collectChildren(EntityId rootEntityId) const;
    void propagateParentState(std::vector<ChildNode>& childList);
    void updateHidden(bool flag, std::vector<ChildNode>& childList);
    void updateZIndex(int newZIndex, std::vector<ChildNode>& childList);
    void updateAlphaMult(float newAlphaMult, std::vector<ChildNode>& childList);
    void updateNormScale(float newNormScale, std::vector<ChildNode>& childList);

private:

    std::vector<ChildNode> manulChildren;
    std::vector<ChildNode> children;
    RenderSceneParams params;
};

#endif /* __RENDER_SCENE_HPP__ */