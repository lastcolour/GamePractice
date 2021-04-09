#ifndef __GAME_BOARD_ELEMS_POOL_HPP__
#define __GAME_BOARD_ELEMS_POOL_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElemsPool.hpp"
#include "Math/Random.hpp"

class ElementDescriptor {
public:

    static void Reflect(ReflectContext& ctx);

public:

    ElementDescriptor();

public:

    std::string object;
    int weight;
};

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
    EntityId ET_spawnElem() override;
    void ET_removeElem(EntityId elemId) override;
    void ET_addElemsToSpawn(const ElementDescriptor& newElemDescr) override;

private:

    void initGenerator();

private:

    struct ElemInfo {
        int poolId;
        bool isAlive;
    };

private:

    Math::RandomIntGenerator intGenerator;
    std::unordered_map<EntityId, ElemInfo> elements;
    std::vector<ElementDescriptor> poolsInfo;
};

#endif /* __GAME_BOARD_ELEMS_POOL_HPP__ */