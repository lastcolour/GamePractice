#include "GameInitModule.hpp"
#include "Core/SystemLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"
#include "UI/UIETInterfaces.hpp"

namespace {

const char* GAME_CONFIG_FILE = "Game/GameConfig.json";

class GameInitLogic : public SystemLogic,
    ETNode<ETSurfaceEvents> {
public:

    virtual ~GameInitLogic() = default;
    
    // SystemLogic
    bool init() override {
        JSONNode node;
        ET_SendEventReturn(node, &ETAssets::ET_loadJSONAsset, GAME_CONFIG_FILE);
        if(!node) {
            LogError("Can't load root game config file: %s", GAME_CONFIG_FILE);
            return false;
        }
        node.value("mainView", mainView);
        if(mainView.empty()) {
            LogError("Empty main view in config file: %s", GAME_CONFIG_FILE);
            return false;
        }

        bool isSurfaceValid = false;
        ET_SendEventReturn(isSurfaceValid, &ETSurface::ET_isValid);
        if(isSurfaceValid) {
            openMainView();
        }
        ETNode<ETSurfaceEvents>::connect(getEntityId());
        return true;
    }

    void deinit() override {}

    // ETSurfaceEvents
    void ET_onSurfaceCreated() override {
        if(!mainViewId.isValid()) {
            openMainView();
        }
    }
    void ET_onSurfaceDestroyed() override {
        ET_SendEvent(&ETUIViewManager::ET_closeView, mainViewId);
        mainViewId = InvalidEntityId;
    }
    void ET_onSurfaceHidden() override {}
    void ET_onSurfaceShown() override {}
    void ET_onSurfaceLostFocus() override {}
    void ET_onSurfaceGainFocus() override {}
    void ET_onSurfaceResized(const Vec2i& size) override { (void)size; }

private:

    void openMainView() {
        ET_SendEventReturn(mainViewId, &ETUIViewManager::ET_openView, mainView.c_str());
        if (!mainViewId.isValid()) {
            LogError("Can't cretae main view '%s' from config file: %s", mainView, GAME_CONFIG_FILE);
        }
        bool isVisible = false;
        ET_SendEventReturn(isVisible, &ETSurface::ET_isVisible);
        if(!isVisible) {
            ET_SendEvent(&ETSurface::ET_show);
        }
    }

private:

    EntityId mainViewId;
    std::string mainView;
};

} // namespace


GameInitModule::LogicsContainerPtrT GameInitModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            GameInitLogic>()
        );
    return container;
}