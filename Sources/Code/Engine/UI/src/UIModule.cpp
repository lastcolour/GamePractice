#include "UI/UIModule.hpp"
#include "UISurfaceEventHandler.hpp"
#include "UIEventManager.hpp"
#include "UIViewStack.hpp"
#include "UIViewSwitcher.hpp"
#include "UIConfig.hpp"
#include "Logics/UIBox.hpp"
#include "Logics/UILayout.hpp"
#include "Logics/UIView.hpp"
#include "Logics/UIBoxVisual.hpp"
#include "Entity/EntityLogicsRegister.hpp"

UIModule::UIModule() :
    SystemModule("UI") {}

UIModule::LogicsContainerPtrT UIModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            UISurfaceEventHandler,
            UIEventManager,
            UIViewStack,
            UIViewSwitcher>()
        );
    return container;
}

void UIModule::reflectSystemConfigs(ReflectContext& ctx) const {
    ctx.reflect<UIConfig>();
}

void UIModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
    logicsRegister.registerLogic<UIBox>();
    logicsRegister.registerLogic<UILayout>();
    logicsRegister.registerLogic<UIView>();
    logicsRegister.registerLogic<UIBoxVisual>();
}