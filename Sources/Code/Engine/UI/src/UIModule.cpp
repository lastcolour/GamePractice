#include "UI/UIModule.hpp"
#include "UISurfaceEventHandler.hpp"
#include "UIViewManager.hpp"
#include "UIEventManager.hpp"
#include "UIConfig.hpp"
#include "UITimer.hpp"
#include "UIViewTransitionManager.hpp"
#include "UIViewPortManager.hpp"
#include "UIViewCache.hpp"
#include "LoadingScreenManager.hpp"
#include "Logics/UIBox.hpp"
#include "Logics/UILayout.hpp"
#include "Logics/UIView.hpp"
#include "Logics/UIButton.hpp"
#include "Logics/UIPressAnimation.hpp"
#include "Logics/UILabel.hpp"
#include "Logics/UIViewAppearAnimation.hpp"
#include "Logics/UIGameEndStarsAppearAnimation.hpp"
#include "Logics/UIGameEndViewScript.hpp"
#include "Entity/EntityLogicsRegister.hpp"

UIModule::UIModule() :
    SystemModule("UI") {}

UIModule::LogicsContainerPtrT UIModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            UISurfaceEventHandler,
            UIViewManager,
            UIEventManager,
            UITimer,
            UIViewTransitionManager,
            UIViewPortManager,
            UIViewCache,
            LoadingScreenManager>()
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
    logicsRegister.registerLogic<UIButton>();
    logicsRegister.registerLogic<UIPressAnimation>();
    logicsRegister.registerLogic<UILabel>();
    logicsRegister.registerLogic<UIViewAppearAnimation>();
    logicsRegister.registerLogic<UIGameEndViewScript>();
    logicsRegister.registerLogic<UIGameEndStarsAppearAnimation>();
}