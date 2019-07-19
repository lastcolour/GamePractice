#include "GameInitModule.hpp"
#include "Core/SystemLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Core/JSONNode.hpp"

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
        std::string rootObjectName;
        node.value("rootObject", rootObjectName);
        if(rootObjectName.empty()) {
            LogError("Empty root object in config file: %s", GAME_CONFIG_FILE);
            return false;
        }
        EntityId rootObjId;
        ET_SendEventReturn(rootObjId, &ETGameObjectManager::ET_createGameObject, rootObjectName);
        if (!rootObjId.isValid()) {
            LogError("Can't cretae root object '%s' from config file: %s", rootObjectName, GAME_CONFIG_FILE);
            return false;
        }
        ET_SendEvent(&ETSurface::ET_show);
        return true;
    }

    void deinit() override {}

private:

    EntityId rootGameObjectId;
};

} // namespace


GameInitModule::LogicsContainerPtrT GameInitModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            GameInitLogic>()
        );
    return container;
}