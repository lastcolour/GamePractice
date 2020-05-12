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
    logicRegister.registerLogic<UIBox>("UIBox");
    logicRegister.registerLogic<UIList>("UIList");
    logicRegister.registerLogic<UIButton>("UIButton");
    logicRegister.registerLogic<UILabel>("UILabel");
    logicRegister.registerLogic<UIBaseBox>("UIBaseBox");
    logicRegister.registerLogic<UIImage>("UIImage");
    logicRegister.registerLogic<UIButtonPressAnimation>("UIButtonPressAnimation");
    logicRegister.registerLogic<UILabelSetter>("UILabelSetter");
    logicRegister.registerLogic<UIResultInfoSelector>("UIResultInfoSelector");
    logicRegister.registerLogic<UISwitchControl>("UISwitchControl");
    logicRegister.registerLogic<UIAppearOnTop>("UIAppearOnTop");
    logicRegister.registerLogic<UIAppearFromSide>("UIAppearFromSide");
    logicRegister.registerLogic<UIBackground>("UIBackground");
}
