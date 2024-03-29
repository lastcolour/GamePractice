#include "UI/UIModule.hpp"
#include "UISurfaceEventHandler.hpp"
#include "UIViewManager.hpp"
#include "Config/UIConfig.hpp"
#include "UITimer.hpp"
#include "UIViewTransitionManager.hpp"
#include "UIViewPortManager.hpp"
#include "UIViewCache.hpp"
#include "LoadingScreenManager.hpp"
#include "Logics/UIBox.hpp"
#include "Logics/UILayout.hpp"
#include "Logics/UIView.hpp"
#include "Logics/UIButton.hpp"
#include "Logics/UILabel.hpp"
#include "Logics/UIScrollArea.hpp"
#include "Logics/UILayoutBox.hpp"
#include "Logics/UIAnimationSequence.hpp"
#include "Logics/UIElementProxy.hpp"
#include "Logics/UIScrollFocus.hpp"
#include "Logics/UIProgressBar.hpp"
#include "Entity/EntityLogicsRegister.hpp"
#include "UIReAlignManager.hpp"

UIModule::UIModule() :
    SystemModule("UI") {}

UIModule::LogicsContainerPtrT UIModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            UISurfaceEventHandler,
            UIViewManager,
            UITimer,
            UIViewTransitionManager,
            UIViewPortManager,
            UIViewCache,
            LoadingScreenManager,
            UIReAlignManager>()
        );
    return container;
}

void UIModule::createSystemConfigs() const {
    Core::CreateGlobal<UIConfig>("Config/UI.json");
}

void UIModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
    logicsRegister.registerLogic<UIBox>();
    logicsRegister.registerLogic<UILayout>();
    logicsRegister.registerLogic<UIView>();
    logicsRegister.registerLogic<UIButton>();
    logicsRegister.registerLogic<UILabel>();
    logicsRegister.registerLogic<UIScrollArea>();
    logicsRegister.registerLogic<UILayoutBox>();
    logicsRegister.registerLogic<UIAnimationSequence>();
    logicsRegister.registerLogic<UIElementProxy>();
    logicsRegister.registerLogic<UIScrollFocus>();
    logicsRegister.registerLogic<UIProgressBar>();
}