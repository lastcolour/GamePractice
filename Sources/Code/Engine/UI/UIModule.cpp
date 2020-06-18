#include "UI/UIModule.hpp"
#include "UI/UISurfaceEventHandler.hpp"
#include "UI/UIEventManager.hpp"
#include "UI/UIViewStack.hpp"
#include "UI/UIViewSwitcher.hpp"
#include "UI/UIConfig.hpp"
#include "UI/Logics/UIBox.hpp"
#include "UI/Logics/UILayout.hpp"
#include "UI/Logics/UIView.hpp"
#include "UI/Logics/UIBoxVisual.hpp"
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