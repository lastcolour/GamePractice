#ifndef __ET_VISUAL_EFFECT_HPP__
#define __ET_VISUAL_EFFECT_HPP__

#include "Math/Transform.hpp"

struct VisualEffectSpawnConfig {
    Transform tm;
};

struct ETVisualEffectSpawner {
    virtual ~ETVisualEffectSpawner() = default;
    virtual void ET_spawn(const VisualEffectSpawnConfig& spawnConfig) = 0;
};

#endif /* __ET_VISUAL_EFFECT_HPP__ */