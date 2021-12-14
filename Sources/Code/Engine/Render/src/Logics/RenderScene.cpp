#include "Logics/RenderScene.hpp"
#include "Render/ETRenderNode.hpp"
#include "Reflect/EnumInfo.hpp"

#include <cassert>

void RenderSceneParams::StencilOcclusion::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<RenderSceneParams::EOcclusionPolicy>("EOcclusionPolicy")) {
        enumInfo->addValues<RenderSceneParams::EOcclusionPolicy>({
            {"None", RenderSceneParams::EOcclusionPolicy::None},
            {"OccludAll", RenderSceneParams::EOcclusionPolicy::OccludAll}
        });
    }
    if(auto classInfo = ctx.classInfo<RenderSceneParams::StencilOcclusion>("StencilOcclusion")) {
        classInfo->addField("refOp", &RenderSceneParams::StencilOcclusion::refOp);
        classInfo->addField("refVal", &RenderSceneParams::StencilOcclusion::refVal);
    }
}

void RenderSceneParams::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderSceneParams>("RenderSceneParams")) {
        classInfo->addField("occlussion", &RenderSceneParams::occlusion);
    }
}

void RenderScene::ChildNode::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderScene::ChildNode>("RenderSceneChildNode")) {
        classInfo->addField("entId", &RenderScene::ChildNode::entId);
        classInfo->addField("zIndexOffset", &RenderScene::ChildNode::zIndexOffset);
        classInfo->addField("propagateVisibility", &RenderScene::ChildNode::propagateVisibility);
    }
}

void RenderScene::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderScene>("RenderScene")) {
        classInfo->addField("params", &RenderScene::params);
        classInfo->addField("manualChildren", &RenderScene::manulChildren);
    }
}

RenderScene::RenderScene() {
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

void RenderScene::ET_onLoaded() {
    ETNode<ETEntityEvents>::disconnect();
    {
        bool res = false;
        ET_SendEventReturn(res, getEntityId(), &ETRenderNode::ET_setEmitEvents, true);
        if(!res) {
            LogWarning("[RenderScene::ET_onLoaded] Can't find 'ETRenderNode' on entity '%s'",
                EntityUtils::GetEntityName(getEntityId()));
        }
    }

    switch(params.occlusion.refOp) {
        case RenderSceneParams::EOcclusionPolicy::None: {
            StencilWirteReadData stencilData;
            stencilData.opType = EStencilOpType::Disabled;
            stencilData.refVal = 0;
            ET_SendEvent(getEntityId(), &ETRenderNode::ET_setStencilData, stencilData);
            break;
        }
        case RenderSceneParams::EOcclusionPolicy::OccludAll: {
            StencilWirteReadData stencilData;
            stencilData.opType = EStencilOpType::Write;
            stencilData.refVal = params.occlusion.refVal;
            ET_SendEvent(getEntityId(), &ETRenderNode::ET_setStencilData, stencilData);
            break;
        }
        default: {
            assert(false && "Invalid stencil op");
        }
    }

    propagateParentState(manulChildren);
}

void RenderScene::ET_addItem(int zIndexOffset, EntityId entId) {
    if(!entId.isValid()) {
        LogError("[RenderScene::ET_addItem] Can't add invalid entity to entity: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        return;
    }
    if(getEntityId() == entId) {
        LogError("[RenderScene::ET_addItem] Can't add self to scene: '%s'",
            EntityUtils::GetEntityName(getEntityId()));
        return;
    }
    for(auto& child : children) {
        if(child.entId == entId) {
            LogWarning("[RenderScene::ET_addItem] Entity '%s' already added to scene on entity: '%s'",
                EntityUtils::GetEntityName(entId), EntityUtils::GetEntityName(getEntityId()));
            return;
        }
    }
    for(auto& child : manulChildren) {
        if(child.entId == entId) {
            LogWarning("[RenderScene::ET_addItem] Entity '%s' already manually added to scene on entity: '%s'",
                EntityUtils::GetEntityName(entId), EntityUtils::GetEntityName(getEntityId()));
            return;
        }
    }

    auto newChildren = collectChildren(entId);
    for(auto& newChild : newChildren) {
        newChild.zIndexOffset = (newChild.zIndexOffset + 1) * zIndexOffset;
    }

    propagateParentState(newChildren);
    children.insert(children.end(), newChildren.begin(), newChildren.end());
}

void RenderScene::ET_removeItem(EntityId entId) {
    std::vector<ChildNode> childToRemove = collectChildren(entId);

    if(params.occlusion.refOp != RenderSceneParams::EOcclusionPolicy::None) {
        StencilWirteReadData defStencilData;
        for(auto& node : childToRemove) {
            ET_SendEvent(node.entId, &ETRenderNode::ET_setStencilData, defStencilData);
        }
    }

    children.erase(std::remove_if(children.begin(), children.end(),
        [&childToRemove](const ChildNode& node) {
            return childToRemove.end() != std::find_if(childToRemove.begin(), childToRemove.end(),
                [&node](const ChildNode& otherNode){
                    return otherNode.entId == node.entId;
            });
    }), children.end());

    manulChildren.erase(std::remove_if(manulChildren.begin(), manulChildren.end(),
        [&childToRemove](const ChildNode& node) {
            return childToRemove.end() != std::find_if(childToRemove.begin(), childToRemove.end(),
                [&node](const ChildNode& otherNode){
                    return otherNode.entId == node.entId;
            });
    }), manulChildren.end());
}

const RenderSceneParams& RenderScene::ET_getParams() const {
    return params;
}

void RenderScene::ET_setParams(RenderSceneParams& newParams) {
    params = newParams;
    propagateParentState(children);
    propagateParentState(manulChildren);
}

size_t RenderScene::ET_getItemsCount() const {
    return manulChildren.size() + children.size();
}

std::vector<RenderScene::ChildNode> RenderScene::collectChildren(EntityId rootEntityId) const {
    std::vector<ChildNode> result;

    int currentDepth = 0;

    std::vector<EntityId> extraChildren;
    std::vector<EntityId> openList;
    openList.push_back(rootEntityId);

    while(!openList.empty()) {
        size_t sz = openList.size();
        for(size_t i = 0; i < sz; ++i) {
            auto childEntId = openList[i];

            if(!ET_IsExistNode<ETRenderNode>(childEntId)) {
                continue;
            }

            bool childVisible = false;
            ET_SendEventReturn(childVisible, childEntId, &ETRenderNode::ET_isVisible);

            bool propagateVisibility = true;
            result.emplace_back(ChildNode{childEntId, currentDepth, propagateVisibility, childVisible});

            if(!ET_IsExistNode<ETRenderScene>(childEntId)) {
                ET_SendEventReturn(extraChildren, childEntId, &ETEntity::ET_getChildren);
                openList.insert(openList.end(), extraChildren.begin(), extraChildren.end());
                extraChildren.clear();
            }
        }

        openList.erase(openList.begin(), openList.begin() + sz);
        ++currentDepth;
    }

    return result;
}

void RenderScene::propagateParentState(std::vector<ChildNode>& childList) {
    if(childList.empty()) {
        return;
    }

    int rootZIndex = 0;
    ET_SendEventReturn(rootZIndex, getEntityId(), &ETRenderNode::ET_getZIndex);
    updateZIndex(rootZIndex, childList);

    bool rootIsVisible = true;
    ET_SendEventReturn(rootIsVisible, getEntityId(), &ETRenderNode::ET_isVisible);
    updateHidden(!rootIsVisible, childList);

    float rootNormScale = 1.f;
    ET_SendEventReturn(rootNormScale, getEntityId(), &ETRenderNode::ET_getNormalizationScale);
    updateNormScale(rootNormScale, childList);

    float rootAlphaMult = 1.f;
    ET_SendEventReturn(rootAlphaMult, getEntityId(), &ETRenderNode::ET_getAlphaMultiplier);
    updateAlphaMult(rootAlphaMult, childList);

    switch(params.occlusion.refOp) {
        case RenderSceneParams::EOcclusionPolicy::None: {
            StencilWirteReadData stencilData;
            stencilData.opType = EStencilOpType::Disabled;
            stencilData.refVal = 0;
            for(auto& child : childList) {
                ET_SendEvent(child.entId, &ETRenderNode::ET_setStencilData, stencilData);
            }
            break;
        }
        case RenderSceneParams::EOcclusionPolicy::OccludAll: {
            StencilWirteReadData stencilData;
            stencilData.opType = EStencilOpType::Read;
            stencilData.refVal = params.occlusion.refVal;
            for(auto& child : childList) {
                ET_SendEvent(child.entId, &ETRenderNode::ET_setStencilData, stencilData);
            }
            break;
        }
        default: {
            assert(false && "Invalid stenicl op");
        }
    }
}

void RenderScene::updateHidden(bool flag, std::vector<ChildNode>& childList) {
    if(flag) {
        for(auto& child : childList) {
            ET_SendEventReturn(child.prevVisible, child.entId, &ETRenderNode::ET_isVisible);
            ET_SendEvent(child.entId, &ETRenderNode::ET_hide);
        }
    } else {
        for(auto& child : childList) {
            if(child.prevVisible && child.propagateVisibility) {
                ET_SendEvent(child.entId, &ETRenderNode::ET_show);
            }
        }
    }
}

void RenderScene::ET_onHidden(bool flag) {
    updateHidden(flag, children);
    updateHidden(flag, manulChildren);
}

void RenderScene::ET_onZIndexChanged(int newZIndex) {
    updateZIndex(newZIndex, children);
    updateZIndex(newZIndex, manulChildren);
}

void RenderScene::ET_onAlphaMultChanged(float newAlphaMult) {
    updateAlphaMult(newAlphaMult, children);
    updateAlphaMult(newAlphaMult, manulChildren);
}

void RenderScene::ET_onNormScaleChanged(float newNormScale) {
    updateNormScale(newNormScale, children);
    updateNormScale(newNormScale, manulChildren);
}

void RenderScene::updateZIndex(int newZIndex, std::vector<ChildNode>& childList) {
    for(auto& child : childList) {
        int zIndex = newZIndex + child.zIndexOffset;
        ET_SendEvent(child.entId, &ETRenderNode::ET_setZIndex, zIndex);
    }
}

void RenderScene::updateAlphaMult(float newAlphaMult, std::vector<ChildNode>& childList) {
    for(auto& child : childList) {
        ET_SendEvent(child.entId, &ETRenderNode::ET_setAlphaMultiplier, newAlphaMult);
    }
}

void RenderScene::updateNormScale(float newNormScale, std::vector<ChildNode>& childList) {
    for(auto& child : childList) {
        ET_SendEvent(child.entId, &ETRenderNode::ET_setNormalizationScale, newNormScale);
    }
}