#include "UIViewTransitionManager.hpp"
#include "UI/ETUIView.hpp"
#include "UI/ETUIBox.hpp"
#include "Logics/UIAnimationSequence.hpp"
#include "UIUtils.hpp"

#include <cassert>

UIViewTransitionManager::UIViewTransitionManager() {
}

UIViewTransitionManager::~UIViewTransitionManager() {
}

bool UIViewTransitionManager::init() {
    ETNode<ETUIAnimationSequenceEvent>::connect(getEntityId());
    ETNode<ETUIViewTransitionManager>::connect(getEntityId());
    return true;
}

void UIViewTransitionManager::deinit() {
}

void UIViewTransitionManager::ET_onAnimationPlayed(EntityId sourceId, EAnimSequenceType animType) {
    assert(!tasks.empty() && "Empty tasks");

    auto& currentTask = tasks.front();
    auto viewId = currentTask.viewId;

    if(animType == EAnimSequenceType::Appear) {
        assert(currentTask.isAppearing && "invalid task type");

        tasks.erase(tasks.begin());
        ET_SendEvent(&ETUIViewManager::ET_onViewAppeared, viewId);
        startNextTask();
    } else {
        assert(!currentTask.isAppearing && "invalid task type");

        tasks.erase(tasks.begin());
        ET_SendEvent(&ETUIViewManager::ET_onViewDisappeared, viewId);
        startNextTask();
    }
}

void UIViewTransitionManager::ET_addAppearing(EntityId viewId) {
    AppearTask task;
    task.viewId = viewId;
    task.isAppearing = true;
    tasks.push_back(task);
    if(tasks.size() == 1) {
        startNextTask();
    }
}

void UIViewTransitionManager::ET_addDisappearing(EntityId viewId) {
    AppearTask task;
    task.viewId = viewId;
    task.isAppearing = false;
    tasks.push_back(task);
    if(tasks.size() == 1) {
        startNextTask();
    }
}

void UIViewTransitionManager::startNextTask() {
    while(!tasks.empty()) {
        auto task = tasks.front();
        bool animStarted = false;
        if(task.isAppearing) {
            ET_SendEvent(&ETUIViewManager::ET_onViewStartAppearing, task.viewId);
            animStarted = UI::PlayAnimation(task.viewId, EAnimSequenceType::Appear, getEntityId());
        } else {
            animStarted = UI::PlayAnimation(task.viewId, EAnimSequenceType::Disappear, getEntityId());
        }
        if(!animStarted) {
            auto viewId = task.viewId;
            tasks.erase(tasks.begin());
            if(task.isAppearing) {
                ET_SendEvent(task.viewId, &ETUIElement::ET_show);
                ET_SendEvent(&ETUIViewManager::ET_onViewAppeared, task.viewId);
            } else {
                ET_SendEvent(task.viewId, &ETUIElement::ET_hide);
                ET_SendEvent(&ETUIViewManager::ET_onViewDisappeared, task.viewId);
            }
        } else {
            break;
        }
    }
}

bool UIViewTransitionManager::ET_hasActiveTransition() const {
    return !tasks.empty();
}