#include "UI/UIModule.hpp"
#include "UI/UISurfaceEventHandler.hpp"
#include "UI/UIEventManager.hpp"
#include "UI/UIViewStack.hpp"
#include "UI/UIViewSwitcher.hpp"
#include "UI/Logics/UIButton.hpp"
#include "UI/Logics/UIList.hpp"
#include "UI/Logics/UILabel.hpp"
#include "UI/Logics/UIBaseBox.hpp"
#include "UI/Logics/UIImage.hpp"
#include "UI/Logics/UIButtonPressAnimation.hpp"
#include "UI/Logics/UILabelSetter.hpp"
#include "UI/Logics/UIResultInfoSelector.hpp"
#include "UI/Logics/UISwitchControl.hpp"
#include "UI/Logics/UIAppearOnTop.hpp"
#include "UI/Logics/UIAppearFromSide.hpp"
#include "UI/Logics/UIBackground.hpp"
#include "Entity/EntityLogicRegister.hpp"

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
}

void UIModule::registerEntityLogics(EntityLogicRegister& logicRegister) const {
    logicRegister.registerLogic<UIBox>();
    logicRegister.registerLogic<UIList>();
    logicRegister.registerLogic<UIButton>();
    logicRegister.registerLogic<UILabel>();
    logicRegister.registerLogic<UIBaseBox>();
    logicRegister.registerLogic<UIImage>();
    logicRegister.registerLogic<UIButtonPressAnimation>();
    logicRegister.registerLogic<UILabelSetter>();
    logicRegister.registerLogic<UIResultInfoSelector>();
    logicRegister.registerLogic<UISwitchControl>();
    logicRegister.registerLogic<UIAppearOnTop>();
    logicRegister.registerLogic<UIAppearFromSide>();
    logicRegister.registerLogic<UIBackground>();
}
