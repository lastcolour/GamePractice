#include "Logics/RenderScene.hpp"
#include "Render/ETRenderNode.hpp"
#include "Reflect/EnumInfo.hpp"

void RenderScene::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<EChildStencilPolicy>("EChildStencilPolicy")) {
        enumInfo->addValues<EChildStencilPolicy>({
            {"Read", EChildStencilPolicy::Read},
            {"ReadIncrease", EChildStencilPolicy::ReadIncrease},
            {"None", EChildStencilPolicy::None}
        });
    }
    if(auto classInfo = ctx.classInfo<RenderScene>("RenderScene")) {
        classInfo->addField("zIndexStep", &RenderScene::zIndexStep);
        classInfo->addField("stencilPolicy", &RenderScene::stencilPolicy);
        classInfo->addField("buildChildListOnLoad", &RenderScene::buildChildListOnLoad);
    }
}

RenderScene::RenderScene() :
    zIndexStep(1),
    stencilPolicy(EChildStencilPolicy::None),
    buildChildListOnLoad(true) {
}

RenderScene::~RenderScene() {
}

void RenderScene::init() {
    if(!ET_IsExistNode<ETRenderNode>(getEntityId())) {
        LogWarning("[RenderScene::init] Can't find root render node for a scene on entity: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        return;
    }

    ETNode<ETEntityEvents>::connect(getEntityId());
    ETNode<ETRenderNodeEvents>::connect(getEntityId());
    ETNode<ETRenderScene>::connect(getEntityId());
}

void RenderScene::deinit() {
}

void RenderScene::ET_onTransformChanged(const Transform& newTm) {
}

void RenderScene::ET_onLoaded() {
    ETNode<ETEntityEvents>::disconnect();

    if(!buildChildListOnLoad) {
        return;
    }

    std::vector<EntityId> openList;
    ET_SendEventReturn(openList, getEntityId(), &ETEntity::ET_getChildren);

    int currentDepth = 1;

    while(!openList.empty()) {
        size_t sz = openList.size();
        for(size_t i = 0; i < sz; ++i) {
            auto childEntId = openList[i];

            if(!ET_IsExistNode<ETRenderNode>(childEntId)) {
                continue;
            }

            children.emplace_back(ChildNode{childEntId, currentDepth});
    
            if(!ET_IsExistNode<ETRenderScene>(childEntId)) {
                std::vector<EntityId> newOpenList;
                ET_SendEventReturn(newOpenList, childEntId, &ETEntity::ET_getChildren);
                openList.insert(openList.begin(), newOpenList.begin(), newOpenList.end());
            }

        }

        openList.erase(openList.begin(), openList.begin() + sz);
        ++currentDepth;
    }

    if(!children.empty()) {
        ET_SendEvent(getEntityId(), &ETRenderNode::ET_setEmitEvents, true);
    } else {
        return;
    }

    int rootZIndex = 0;
    ET_SendEventReturn(rootZIndex, getEntityId(), &ETRenderNode::ET_getZIndex);
    ET_onZIndexChanged(rootZIndex);

    bool rootIsVisible = true;
    ET_SendEventReturn(rootIsVisible, getEntityId(), &ETRenderNode::ET_isVisible);
    ET_onHidden(!rootIsVisible);

    float rootNormScale = 1.f;
    ET_SendEventReturn(rootNormScale, getEntityId(), &ETRenderNode::ET_getNormalizationScale);
    ET_onNormScaleChanged(rootNormScale);

    float rootAlphaMult = 1.f;
    ET_SendEventReturn(rootAlphaMult, getEntityId(), &ETRenderNode::ET_getAlphaMultiplier);
    ET_onAlphaMultChanged(rootAlphaMult);
}

void RenderScene::ET_addItem(int layer, EntityId entId) {
    if(layer < 1) {
        LogError("[RenderScene::ET_addItem] Can't add entity '%s' to layer '%d' on entity: '%s'",
            EntityUtils::GetEntityName(entId), layer, EntityUtils::GetEntityName(getEntityId()));
        return;
    }
    if(!entId.isValid()) {
        LogError("[RenderScene::ET_addItem] Can't add invalid entity to entity: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        return;
    }
    for(auto& child : children) {
        if(child.entId == entId) {
            LogError("[RenderScene::ET_addItem] Entity '%s' already register on scene on entity: '%s'",
                EntityUtils::GetEntityName(entId), EntityUtils::GetEntityName(getEntityId()));
            return;
        }
    }
    if(getEntityId() == entId) {
        LogError("[RenderScene::ET_addItem] Can't add self to scene: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        return;
    }

    int rootZIndex = 0;
    ET_SendEventReturn(rootZIndex, getEntityId(), &ETRenderNode::ET_getZIndex);
    ET_SendEvent(entId, &ETRenderNode::ET_setZIndex, rootZIndex + zIndexStep * layer);

    bool rootIsVisible = true;
    ET_SendEventReturn(rootIsVisible, getEntityId(), &ETRenderNode::ET_isVisible);
    if(rootIsVisible) {
        ET_SendEvent(entId, &ETRenderNode::ET_show);
    } else {
        ET_SendEvent(entId, &ETRenderNode::ET_hide);
    }

    float rootNormScale = 1.f;
    ET_SendEventReturn(rootNormScale, getEntityId(), &ETRenderNode::ET_getNormalizationScale);
    ET_SendEvent(entId, &ETRenderNode::ET_setNormalizationScale, rootNormScale);

    float rootAlphaMult = 1.f;
    ET_SendEventReturn(rootAlphaMult, getEntityId(), &ETRenderNode::ET_getAlphaMultiplier);
    ET_SendEvent(entId, &ETRenderNode::ET_setAlphaMultiplier, rootAlphaMult);

    children.emplace_back(ChildNode{entId, layer});
}

void RenderScene::ET_removeItem(EntityId entId) {
    auto it = children.begin();
    for(auto end = children.end(); it != end; ++it) {
        if(it->entId == entId) {
            children.erase(it);
            break;
        }
    }
}

void RenderScene::ET_onHidden(bool flag) {
    if(flag) {
        for(auto& child : children) {
            ET_SendEvent(child.entId, &ETRenderNode::ET_hide);
        }
    } else {
        for(auto& child : children) {
            ET_SendEvent(child.entId, &ETRenderNode::ET_show);
        }
    }
}

void RenderScene::ET_onZIndexChanged(int newZIndex) {
    for(auto& child : children) {
        int zIndex = newZIndex + child.depth * zIndexStep;
        ET_SendEvent(child.entId, &ETRenderNode::ET_setZIndex, zIndex);
    }
}

void RenderScene::ET_onAlphaMultChanged(float newAlphaMult) {
    for(auto& child : children) {
        ET_SendEvent(child.entId, &ETRenderNode::ET_setAlphaMultiplier, newAlphaMult);
    }
}

void RenderScene::ET_onNormScaleChanged(float newNormScale) {
    for(auto& child : children) {
        ET_SendEvent(child.entId, &ETRenderNode::ET_setNormalizationScale, newNormScale);
    }
}