#ifndef __ET_RENDER_SCENE_HPP__
#define __ET_RENDER_SCENE_HPP__

struct RenderSceneParams {
public:

    static void Reflect(ReflectContext& ctx);

public:

    enum class EOcclusionPolicy {
        None = 0,
        OccludAll
    };

    struct StencilOcclusion {
    public:

        static void Reflect(ReflectContext& ctx);

    public:

        int refVal{0};
        EOcclusionPolicy refOp{EOcclusionPolicy::None};
    };

public:

    StencilOcclusion occlusion;
};

struct ETRenderScene {
    virtual ~ETRenderScene() = default;
    virtual void ET_addItem(int zIndexOffset, EntityId entId) = 0;
    virtual void ET_removeItem(EntityId entId) = 0;
    virtual const RenderSceneParams& ET_getParams() const = 0;
    virtual void ET_setParams(RenderSceneParams& newParams) = 0;
    virtual size_t ET_getItemsCount() const = 0;
};

#endif /* __ET_RENDER_SCENE_HPP__ */