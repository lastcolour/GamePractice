#include "UI/UIModule.hpp"
#include "UI/UISurfaceTouchManager.hpp"
#include "UI/UIEventManager.hpp"
#include "UI/UIViewManager.hpp"

UIModule::UIModule() :
    SystemModule("UI") {}

UIModule::LogicsContainerPtrT UIModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            UISurfaceTouchManager,
            UIEventManager,
            UIViewManager>()
        );
    return container;
}