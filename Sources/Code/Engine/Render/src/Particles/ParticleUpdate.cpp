#include "Particles/ParticleUpdate.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Math/MatrixTransform.hpp"

#include <algorithm>
#include <cassert>

namespace {

float getRandomFloatInRange(Math::RandomFloatGenerator& gen, float start, float end) {
    float val = gen.generate();
    return Math::Lerp(start, end, val);
}

float getRandomSpeed(Math::RandomFloatGenerator& gen, float val, float var) {
    val += val * getRandomFloatInRange(gen, -var, var);
    val = std::max(0.f, val);
    return val;
}

float getRandomRotation(Math::RandomFloatGenerator& gen, float val, float var) {
    val += var * getRandomFloatInRange(gen, -var, var);
    return val;
}

Vec2 getRandomScale(Math::RandomFloatGenerator& gen, const Vec2& scale, const Vec2& scaleVar) {
    Vec2 resScale = scale;
    resScale.x += resScale.x * getRandomFloatInRange(gen, -scaleVar.x, scaleVar.x);
    resScale.y += resScale.y * getRandomFloatInRange(gen, -scaleVar.y, scaleVar.y);

    resScale.x = std::max(0.001f, resScale.x);
    resScale.y = std::max(0.001f, resScale.y);
    return resScale;
}

ColorF getRandomColor(Math::RandomFloatGenerator& gen, const ColorF& col, const Vec4& colVar) {
    ColorF resCol = col;

    resCol.r += resCol.r * getRandomFloatInRange(gen, -colVar.x, colVar.x);
    resCol.g += resCol.g * getRandomFloatInRange(gen, -colVar.y, colVar.y);
    resCol.b += resCol.b * getRandomFloatInRange(gen, -colVar.z, colVar.z);
    resCol.a += resCol.a * getRandomFloatInRange(gen, -colVar.w, colVar.w);

    resCol.r = Math::Clamp(resCol.r, 0.f, 1.f);
    resCol.g = Math::Clamp(resCol.g, 0.f, 1.f);
    resCol.b = Math::Clamp(resCol.b, 0.f, 1.f);
    resCol.a = Math::Clamp(resCol.a, 0.f, 1.f);
    return resCol;
}

} // namespace

EmitterState::EmitterState() :
    duration(0.f),
    emitFracTime(0.f),
    activeCount(0),
    lifeType(EmitterLifeType::StartDelay),
    floatGen(0.f, 1.f) {
}

void EmitterState::reset() {
    duration = 0.f;
    emitFracTime = 0.f;
    activeCount = 0;
    lifeType = EmitterLifeType::StartDelay;
}

void EmitterState::removeOld(float dt) {
    size_t i = 0;
    while(i < activeCount) {
        auto& p = particles[i];
        assert(p.lifetime > 0.f && "Invalid alive particle");
        p.lifetime -= dt;
        if(p.lifetime <= 0) {
            p.lifetime = 0.f;
            std::swap(particles[i], particles[activeCount - 1]);
            std::swap(instaceData[i], instaceData[activeCount - 1]);
            --activeCount;
        } else {
            ++i;
        }
    }
}

void EmitterState::spawnNewParticle(const Transform& tm, Particle& p) {
    Vec2 moveDir(0.f);
    Vec2 startPt(0.f);

    switch(emissionConfig.emitterType) {
        case EmitterType::Sphere: {
            float shpereR = emissionConfig.emitterVal.x;
            float radius = getRandomFloatInRange(floatGen, 0.f, shpereR);
            float angel = getRandomFloatInRange(floatGen, 0.f, 2.f * Math::PI);

            Vec2 dir;
            dir.x = radius * cos(angel);
            dir.y = radius * sin(angel);

            startPt += dir;

            moveDir = dir.getNormilized();
            break;
        }
        case EmitterType::Box: {
            Vec2 ptInBox;
            ptInBox.x = getRandomFloatInRange(floatGen, -emissionConfig.emitterVal.x, emissionConfig.emitterVal.x) / 2.f;
            ptInBox.y = getRandomFloatInRange(floatGen, -emissionConfig.emitterVal.y, emissionConfig.emitterVal.y) / 2.f;

            Vec2 dir = ptInBox - startPt;
            moveDir = dir.getNormilized();

            startPt += ptInBox;
            break;
        }
        case EmitterType::Cone: {
            float shpereR = emissionConfig.emitterVal.x;
            float radius = getRandomFloatInRange(floatGen, 0.f, shpereR);
            float angel = getRandomFloatInRange(floatGen, 0.f, Math::Deg2Rad(emissionConfig.emitterVal.y));

            Vec2 dir;
            dir.x = radius * cos(angel);
            dir.y = radius * sin(angel);

            startPt += dir;

            moveDir = dir.getNormilized();
            break;
        }
        default: {
            assert(false && "Invalid emitter type");
        }
    }

    p.pt = startPt;
    p.lifetime = emissionConfig.lifetime;
    p.totalLifetime = p.lifetime;

    p.startSpeed = moveDir * getRandomSpeed(floatGen, movementConifg.startSpeed, movementConifg.startSpeedVar);
    p.endSpeed = moveDir * getRandomSpeed(floatGen, movementConifg.endSpeed, movementConifg.endSpeedVar);

    p.startScale = getRandomScale(floatGen, movementConifg.startScale, movementConifg.startScaleVar);
    p.endScale = getRandomScale(floatGen, movementConifg.endScale, movementConifg.endScaleVar);

    p.rot = getRandomRotation(floatGen, movementConifg.startRotation, movementConifg.startRotationVar);
    p.startRotSpeed = getRandomRotation(floatGen, movementConifg.startRotSpeed, movementConifg.startRotSpeedVar);
    p.endRotSpeed = getRandomRotation(floatGen, movementConifg.endRotSpeed, movementConifg.endRotSpeedVar);

    p.startCol = getRandomColor(floatGen, colorConfig.startCol.getColorF(), colorConfig.startColVar);
    p.endCol = getRandomColor(floatGen, colorConfig.endCol.getColorF(), colorConfig.endColVar);

    if(emissionConfig.emitterSpace == EmitterSpace::World) {
        p.pt.x *= tm.scale.x;
        p.pt.y *= tm.scale.y;

        p.pt += Vec2(tm.pt.x, tm.pt.y);
        p.startScale.x *= tm.scale.x;
        p.startScale.y *= tm.scale.y;

        p.endScale.x *= tm.scale.x;
        p.endScale.y *= tm.scale.y;
    }
}

void EmitterState::emitNew(const Transform& tm, float dt) {
    emitFracTime += dt;

    auto prog = std::max(0.f, duration / emissionConfig.duration);
    float emissionRate = Math::Lerp(emissionConfig.startEmissionRate, emissionConfig.endEmissionRate, prog);

    int emitCount = static_cast<int>(emissionRate * emitFracTime);
    emitCount = std::min(Render::MaxParticlessPerDraw, activeCount + emitCount) - activeCount;

    emitFracTime -= emitCount / emissionRate;

    int newCount = std::max(((activeCount + emitCount) - static_cast<int>(particles.size())), 0);
    for(int i = 0; i < newCount; ++i) {
        particles.emplace_back();
        instaceData.emplace_back();
    }

    for(int i = activeCount, sz = activeCount + emitCount; i < sz; ++i) {
        auto& p = particles[i];
        spawnNewParticle(tm, p);
    }

    activeCount += emitCount;
    assert(activeCount <= Render::MaxParticlessPerDraw && "Too Many Particles");
}

void EmitterState::updateAlive(float dt) {
    for(int i = 0; i < activeCount; ++i) {
        auto& p = particles[i];
        auto& out = instaceData[i];

        auto prog = 1.f - p.lifetime / p.totalLifetime;
    
        Vec2 speed = Math::Lerp(p.startSpeed, p.endSpeed, prog);
        p.pt += speed * dt;

        Vec2 scale = Math::Lerp(p.startScale, p.endScale, prog);

        float rotSpeed = Math::Lerp(p.startRotSpeed, p.endRotSpeed, prog);
        p.rot += rotSpeed * dt;

        Mat3 mat(1.f);
        Math::AddTranslate2D(mat, p.pt);
        Math::AddRotate2D(mat, p.rot);
        Math::AddScale2D(mat, 20.f * scale);

        out.mat = mat.toMat3x2();

        out.col.r = Math::Lerp(p.startCol.r, p.endCol.r, prog);
        out.col.g = Math::Lerp(p.startCol.g, p.endCol.g, prog);
        out.col.b = Math::Lerp(p.startCol.b, p.endCol.b, prog);
        out.col.a = Math::Lerp(p.startCol.a, p.endCol.a, prog);
    }
}

void EmitterState::updateState(float dt) {
    switch(lifeType) {
        case EmitterLifeType::StartDelay: {
            duration += dt;
            if(duration > emissionConfig.startDelay) {
                lifeType = EmitterLifeType::Alive;
                duration = 0.f;
                emitFracTime = 0.f;
            }
            break;
        }
        case EmitterLifeType::Alive: {
            if(emissionConfig.duration < 0.f) {
                break;
            }
            duration += dt;
            if(duration > emissionConfig.duration) {
                duration = 0.f;
                emitFracTime = 0.f;
                lifeType = EmitterLifeType::WaitingAlive;
            }
            break;
        }
        case EmitterLifeType::WaitingAlive: {
            if(activeCount == 0) {
                lifeType = EmitterLifeType::Finished;
            }
            break;
        }
        case EmitterLifeType::Finished: {
            if(emissionConfig.loop) {
                lifeType = EmitterLifeType::StartDelay;
            }
            break;
        }
        default: {
            assert(false && "Invalid emitter life type");
        }
    }
}

void EmitterState::update(const Transform& tm, float dt) {
    updateState(dt);
    if(lifeType == EmitterLifeType::Finished) {
        return;
    }
    if(activeCount > 0) {
        removeOld(dt);
    }
    if(lifeType == EmitterLifeType::Alive) {
        emitNew(tm, dt);
    }
    if(activeCount > 0) {
        updateAlive(dt);
    }
}