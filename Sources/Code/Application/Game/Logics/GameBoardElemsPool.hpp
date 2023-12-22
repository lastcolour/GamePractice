#ifndef __GAME_BOARD_ELEMS_POOL_HPP__
#define __GAME_BOARD_ELEMS_POOL_HPP__

#include "Entity/EntityLogic.hpp"
#include "Entity/EntityResource.hpp"
#include "Game/ETGameElemsPool.hpp"
#include "Math/Random.hpp"

class GameBoardElemsPool : public EntityLogic,
    public ETNode<ETGameBoardElemsPool> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardElemsPool();
    virtual ~GameBoardElemsPool();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameBoardElemsPool
    EntityId ET_spawnBaseRandomElem() override;
    EntityId ET_spawnElem(EBoardElemType elemType) override;
    void ET_removeElem(EntityId elemId) override;
    void ET_setPoolsSetup(const std::vector<GameElemPoolInfo>& newPoolsSetup) override;
    void ET_setElemEntity(EBoardElemType elemType, const char* entityName) override;

private:

    void initPools();
    void cleanUpPools();
    const EntityResource* getEntityResource(EBoardElemType elemType) const;

private:

    struct ElemInfo {
        EntityId entId;
        bool isAlive;
    };

private:

    Math::RandomIntGenerator intGenerator;
    std::vector<GameElemPoolInfo> poolsSetup;
    std::unordered_map<EBoardElemType, std::vector<ElemInfo>> poolsElems;
    EntityResource blueEntity;
    EntityResource redEntity;
    EntityResource yellowEntity;
    EntityResource purpleEntity;
    EntityResource greenEntity;
    EntityResource vRocketEntity;
    EntityResource hRocketEntity;
    EntityResource bombEntity;
    EntityResource starEntity;
};

#endif /* __GAME_BOARD_ELEMS_POOL_HPP__ */