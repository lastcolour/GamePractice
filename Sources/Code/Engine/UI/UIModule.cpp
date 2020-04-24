#include "UI/UIModule.hpp"
#include "UI/UISurfaceEventHandler.hpp"
#include "UI/UIEventManager.hpp"
#include "UI/UIViewStack.hpp"
#include "UI/UIViewSwitcher.hpp"

UIModule::UIModule() :
    SystemModule("UI") {}

UIModule::LogicsContainerPtrT UIModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            UISurfaceEventHandler,
            UIEventManager,
            UIViewStack,
            UIViewSwitcher>()
        );
    return container;
}

UIModule::ConfigsPtrT UIModule::getSystemConfigs() const {
    return nullptr;
}
