#include "UIReAlignManager.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/ETUILayout.hpp"
#include "Core/TimePoint.hpp"

#include <cassert>

namespace {

struct LayoutData {
    EntityId id;
    EntityId parentId;
    int depth{0};
};

} // namespace

UIReAlignManager::UIReAlignManager() :
    resizing(false) {
}

UIReAlignManager::~UIReAlignManager() {
}

bool UIReAlignManager::init() {
    ETNode<ETUIReAlignManager>::connect(getEntityId());
    return true;
}

void UIReAlignManager::deinit() {
    ETNode<ETUIReAlignManager>::disconnect();
}

void UIReAlignManager::ET_setLayoutDirty(EntityId entId) {
    assert(entId.isValid());
    assert(!resizing);

    dirtyLayouts.insert(entId);
}

void UIReAlignManager::ET_doReAlign() {
    if(dirtyLayouts.empty()) {
        return;
    }

    resizing = true;
    auto StartT = TimePoint::GetNow();

    std::unordered_map<EntityId, LayoutData> discoveredLayouts;

    for(auto layoutId : dirtyLayouts) {
        EntityId currEntId = layoutId;
        bool bUpdateDepth = false;
        while(currEntId.isValid()) {
            if(discoveredLayouts.count(currEntId)) {
                break;
            } else {
                bUpdateDepth = true;
                auto prevEntId = currEntId;
                currEntId = InvalidEntityId;
                ET_SendEventReturn(currEntId, prevEntId, &ETUIElement::ET_getHostLayout);
                discoveredLayouts.insert({prevEntId, LayoutData{prevEntId, currEntId, -1}});
            }
        }

        if(bUpdateDepth) {
            int depth = 0;
            LayoutData* layoutData = &discoveredLayouts[layoutId];
            while(layoutData) {
                layoutData->depth = std::max(layoutData->depth, depth);
                depth += 1;
                layoutData = layoutData->parentId.isValid() ? &discoveredLayouts[layoutData->parentId] : nullptr;
            }
        }
    }

    std::vector<LayoutData> alignQueue;
    alignQueue.reserve(dirtyLayouts.size());
    for(const auto& layoutData : discoveredLayouts) {
        bool isHidden = true;
        ET_SendEventReturn(isHidden, layoutData.second.id, &ETUIElement::ET_isHidden);
        if(isHidden) {
            continue;
        }
        alignQueue.push_back(layoutData.second);
    }

    discoveredLayouts.clear();

    std::sort(alignQueue.begin(), alignQueue.end(), [](const LayoutData& a, const LayoutData& b){
        return a.depth < b.depth;
    });

    for(auto& layoutData : alignQueue) {
        ET_SendEvent(layoutData.id, &ETUIElemAligner::ET_reAlign);
    }

    const auto msDelta = StartT.getMsDeltaWithNow();
    if(msDelta > 1.f) {
        LogDebug("[UIReAlignManager::ET_doReAlign] Resized %d entities (%.1f ms)", alignQueue.size(), msDelta);
    }

    dirtyLayouts.clear();

    resizing = false;
}