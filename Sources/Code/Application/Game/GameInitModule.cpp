#include "GameInitModule.hpp"
#include "Core/SystemLogic.hpp"
#include "Game/ETGameInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"

namespace {

const char* ROOT_GAME_OBJECT = "GameBoard";

class GameInitLogic : public SystemLogic {
public:

    virtual ~GameInitLogic() = default;
    
    bool init() override {
        ET_SendEventReturn(rootGameObjectId, &ETGameObjectManager::ET_createGameObject, ROOT_GAME_OBJECT);
        if (rootGameObjectId.isValid()) {
            ET_SendEvent(&ETSurface::ET_show);
        }
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