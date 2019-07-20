#include "GameInitModule.hpp"
#include "Core/SystemLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"
#include "UI/UIETInterfaces.hpp"

namespace {

const char* GAME_CONFIG_FILE = "Game/GameConfig.json";

class GameInitLogic : public SystemLogic {
public:

    virtual ~GameInitLogic() = default;
    
    bool init() override {
        JSONNode node;
        ET_SendEventReturn(node, &ETAssets::ET_loadJSONAsset, GAME_CONFIG_FILE);
        if(!node) {
            LogError("Can't load root game config file: %s", GAME_CONFIG_FILE);
            return false;
        }
        std::string mainView;
        node.value("mainView", mainView);
        if(mainView.empty()) {
            LogError("Empty main view in config file: %s", GAME_CONFIG_FILE);
            return false;
        }
        bool openRes;
        ET_SendEventReturn(openRes, &ETUIViewManager::ET_openView, mainView);
        if (!openRes) {
            LogError("Can't cretae main view '%s' from config file: %s", mainView, GAME_CONFIG_FILE);
            return false;
        }
        ET_SendEvent(&ETSurface::ET_show);
        return true;
    }

    void deinit() override {}
};

} // namespace


GameInitModule::LogicsContainerPtrT GameInitModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            GameInitLogic>()
        );
    return container;
}