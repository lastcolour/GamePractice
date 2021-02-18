#ifndef __VISUAL_EFFECT_SPAWNER_HPP__
#define __VISUAL_EFFECT_SPAWNER_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Game/ETVisualEffect.hpp"

class ReflectContext;

class VisualEffectSpawner : public EntityLogic,
    public ETNode<ETVisualEffectSpawner> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    VisualEffectSpawner();
    virtual ~VisualEffectSpawner();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETVisualEffectSpawner
    void ET_spawn(const VisualEffectSpawnConfig& spawnConfig) override;

private:

    std::string effectEntityName;
    std::vector<EntityId> activeEffects;
};

#endif /* __VISUAL_EFFECT_SPAWNER_HPP__ */