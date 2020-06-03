#include "UI/UIViewStack.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Entity/ETEntityInterfaces.hpp"

#include <cassert>

namespace {

const int Z_INDEX_OFFSET_BUFFER = 256;

} // namespace

UIViewStack::UIViewStack() {
}

UIViewStack::~UIViewStack() {
}

bool UIViewStack::init() {
    ETNode<ETUIViewStack>::connect(getEntityId());
    ETNode<ETUIViewSwitcherEvents>::connect(getEntityId());
    return true;
}

void UIViewStack::deinit() {
    ETNode<ETUIViewStack>::disconnect();
    ETNode<ETUIViewSwitcherEvents>::disconnect();
}

void UIViewStack::ET_pushView(const char* viewName) {
    if(!viewName || !viewName[0]) {
        LogWarning("[UIViewStack::ET_pushView] Can't push view with empty name");
        return;
    }
    if(viewStack.empty()) {
        if(initPush(viewName)) {
            EntityId viewId = ET_getActiveViewId();
            ET_SendEvent(&ETUIViewStackEvents::ET_onViewStartPush, viewId);
            ET_SendEvent(&ETUIViewStackEvents::ET_onViewFinishPush, viewId);
        }
    } else if(taskQueue.empty()) {
        if(!initPush(viewName)) {
            return;
        }
        notifyNormalPush();
        StackTask task;
        task.viewName = viewName;
        task.state = ETaskState::Pushing;
        taskQueue.push_back(task);
    } else if(taskQueue.back().state == ETaskState::Popping && taskQueue.front().viewName == viewName) {
        auto& lastTask = taskQueue.back();
        lastTask.state = ETaskState::Pushing;
        viewStack.push_back(lastTask.viewId);

        EntityId viewId = ET_getActiveViewId();
        ET_SendEvent(&ETUIViewStackEvents::ET_onViewStartPush, viewId);

        ET_SendEvent(&ETUIViewSwitcher::ET_reverse);
    } else {
        StackTask task;
        task.viewName = viewName;
        task.state = ETaskState::WaitPush;
        taskQueue.push_back(task);
    }
}

void UIViewStack::ET_forcePushView(const char* viewName) {
    ET_pushView(viewName);
}

EntityId UIViewStack::ET_getActiveViewId() const {
    if(!viewStack.empty()) {
        return viewStack.back();
    }
    return InvalidEntityId;
}

void UIViewStack::ET_popView() {
    if(taskQueue.empty()) {
        if(viewStack.size() < 2) {
            LogWarning("[UIViewStack::ET_popView] No view to pop from stack");
            return;
        }
        EntityId activeViewId = ET_getActiveViewId();

        notifyNormalPop();

        StackTask task;
        task.state = ETaskState::Popping;
        task.viewId = activeViewId;
        ET_SendEventReturn(task.viewName, task.viewId, &ETEntity::ET_getName);
        taskQueue.push_back(task);
    } else if(taskQueue.back().state == ETaskState::WaitPush) {
        taskQueue.pop_back();
        return;
    } else if(taskQueue.back().state == ETaskState::Pushing) {
        auto& lastTask = taskQueue.back();
        lastTask.state = ETaskState::Popping;
        lastTask.viewId = viewStack.back();
        viewStack.pop_back();

        EntityId viewId = ET_getActiveViewId();
        ET_SendEvent(&ETUIViewStackEvents::ET_onViewStartPop, viewId);

        ET_SendEvent(&ETUIViewSwitcher::ET_reverse);
    } else {
        StackTask task;
        task.state = ETaskState::WaitPop;
        taskQueue.push_back(task);
    }
}

void UIViewStack::ET_onViewSwitchFinished(EntityId viewId) {
    (void)viewId;

    if(taskQueue.empty()) {
        LogWarning("[UIViewStack::ET_onViewSwitchFinished] Can't finish view switch with empty task queue");
        return;
    }
    const auto& activeTask = taskQueue.front();
    if(activeTask.state == ETaskState::Popping) {
        ET_SendEvent(&ETUIViewStackEvents::ET_onViewFinishPop, activeTask.viewId);
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, activeTask.viewId);
    } else if(activeTask.state == ETaskState::Pushing) {
        ET_SendEvent(&ETUIViewStackEvents::ET_onViewFinishPush, activeTask.viewId);
    } else {
        assert(false && "Invalid active task state");
        return;
    }
    taskQueue.erase(taskQueue.begin());
    startNextTask();
}

void UIViewStack::ET_clearAllAndPushNewView(const char* viewName) {
    ET_forceReset();
    ET_pushView(viewName);
}

void UIViewStack::ET_forceReset() {
    ET_SendEvent(&ETUIViewSwitcher::ET_forceSwtichStop);
    for(auto it = viewStack.rbegin(), end = viewStack.rend(); it != end; ++it) {
        EntityId viewId = *it;
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, viewId);
    }
    viewStack.clear();
    taskQueue.clear();
}

bool UIViewStack::initPush(const std::string& viewName) {
    EntityId newViewId;
    ET_SendEventReturn(newViewId, &ETEntityManager::ET_createEntity, viewName.c_str());
    if(!newViewId.isValid()) {
        LogWarning("[UIViewStack::initPush] Can't create view: %s", viewName);
        return false;
    }

    int zIndexOffset = 0;
    if(!viewStack.empty()) {
        auto topViewId = viewStack.back();
        ET_SendEventReturn(zIndexOffset, topViewId, &ETEntity::ET_getMaxChildrenDepth);
        zIndexOffset += Z_INDEX_OFFSET_BUFFER;
    }

    ET_SendEvent(newViewId, &ETUIVisibleElement::ET_setZIndex, zIndexOffset);

    viewStack.push_back(newViewId);
    return true;
}

void UIViewStack::notifyNormalPush() {
    EntityId newViewId = viewStack[viewStack.size() - 1];
    EntityId prevViewId = viewStack[viewStack.size() - 2];

    ET_SendEvent(&ETUIViewStackEvents::ET_onViewStartPush, ET_getActiveViewId());

    ET_SendEvent(&ETUIViewSwitcher::ET_swtichView, newViewId, prevViewId);
}

void UIViewStack::notifyNormalPop() {
    EntityId activeViewId = viewStack.back();
    viewStack.pop_back();
    EntityId prevViewId = viewStack.back();

    ET_SendEvent(&ETUIViewStackEvents::ET_onViewStartPop, activeViewId);

    ET_SendEvent(&ETUIViewSwitcher::ET_reverseSwitchView, activeViewId, prevViewId);
}

void UIViewStack::startNextTask() {
    while(!taskQueue.empty()) {
        auto& nextTask = taskQueue.front();
        if(nextTask.state == ETaskState::WaitPop) {
            if(viewStack.size() > 1) {
                notifyNormalPop();

                nextTask.state = ETaskState::Popping;
                nextTask.viewId = viewStack.back();
                ET_SendEventReturn(nextTask.viewName, nextTask.viewId, &ETEntity::ET_getName);
                break;
            } else {
                LogWarning("[UIViewStack::startNextTask] Can't start next pop without enought views on stack");
            }
        } else if(nextTask.state == ETaskState::WaitPush) {
            if(initPush(nextTask.viewName)) {
                nextTask.state = ETaskState::Pushing;
                notifyNormalPush();
                break;
            }
        } else {
            assert(false && "Invalid pending task state");
        }
        taskQueue.erase(taskQueue.begin());
    }
}