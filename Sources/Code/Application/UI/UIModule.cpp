#include "UI/UIModule.hpp"
#include "UI/UISurfaceTouchManager.hpp"
#include "UI/UIEventManager.hpp"
#include "UI/UIViewManager.hpp"
#include "UI/UIConfigManager.hpp"
#include "UI/UIViewSwitcher.hpp"

UIModule::UIModule() :
    SystemModule("UI") {}

UIModule::LogicsContainerPtrT UIModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            UIConfigManager,
            UISurfaceTouchManager,
            UIEventManager,
            UIViewManager,
            UIViewSwitcher>()
        );
    return container;
}