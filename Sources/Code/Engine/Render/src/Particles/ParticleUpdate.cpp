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
    float rotSpeed = Math::Deg2Rad(val);
    rotSpeed += rotSpeed * getRandomFloatInRange(gen, -var, var);
    return rotSpeed;
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

Vec2 getRandomDir(Math::RandomFloatGenerator& gen, float dir, float dirVar) {
    float radDir = Math::Deg2Rad(dir);
    float radDirVar = Math::Deg2Rad(dirVar);
    radDir += getRandomFloatInRange(gen, -radDirVar, radDirVar);
    return Vec2(cos(radDir), sin(radDir));
}

float getRandomLifetime(Math::RandomFloatGenerator& gen, float val, float var) {
    val += val * getRandomFloatInRange(gen, -var, var);
    val = std::max(0.01f, val);
    return val;
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
    int j = 0;
    for(int i = 0; i < activeCount; ++i) {
        auto& p = particles[i];
        assert(p.lifetime > 0.f && "Invalid alive particle");
        p.lifetime -= dt;
        if(p.lifetime > 0.f) {
            if(i != j) {
                particles[j] = particles[i];
                instaceData[j] = instaceData[i];
            }
            ++j;
        }
    }
    activeCount = j;
}

void EmitterState::spawnNewParticle(const Transform& tm, Particle& p) {
    Vec2 startPt(0.f);

    Vec2 emitterVal = emissionConfig.emitterVal;
    switch(emissionConfig.emitterType) {
        case EmitterType::Sphere: {
            float shpereR = emitterVal.x;
            float radius = getRandomFloatInRange(floatGen, 0.f, shpereR);
            float angel = getRandomFloatInRange(floatGen, 0.f, 2.f * Math::PI);

            startPt.x = radius * cos(angel);
            startPt.y = radius * sin(angel);
            break;
        }
        case EmitterType::Box: {
            startPt.x = getRandomFloatInRange(floatGen, -emitterVal.x, emitterVal.x) / 2.f;
            startPt.y = getRandomFloatInRange(floatGen, -emitterVal.y, emitterVal.y) / 2.f;
            break;
        }
        default: {
            assert(false && "Invalid emitter type");
        }
    }

    p.pt = startPt;
    p.lifetime = getRandomLifetime(floatGen, emissionConfig.lifetime, emissionConfig.lifetimeVar);
    p.totalLifetime = p.lifetime;

    Vec2 moveDir = getRandomDir(floatGen, emissionConfig.direction, emissionConfig.directionVar);

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

        Vec3 speed = tm.quat * Vec3(p.startSpeed, 0.f);
        p.startSpeed = Vec2(speed.x, speed.y);

        speed = tm.quat * Vec3(p.endSpeed, 0.f);
        p.endSpeed = Vec2(speed.x, speed.y);
    }
}

void EmitterState::emitNew(const Transform& tm, float dt) {
    emitFracTime += dt;

    int emitCount = static_cast<int>(emissionConfig.emissionRate * emitFracTime);
    emitCount = std::min(Render::MaxParticlessPerDraw, activeCount + emitCount) - activeCount;
    if(emitCount == 0) {
        return;
    }

    emitFracTime -= emitCount / emissionConfig.emissionRate;

    particles.resize(activeCount + emitCount);
    instaceData.resize(activeCount + emitCount);

    if(activeCount > 0) {
        memmove(&particles[0] + emitCount, &particles[0],
            activeCount * sizeof(Particle));
        memmove(&instaceData[0] + emitCount, &instaceData[0],
            activeCount * sizeof(ParticleInstanceData));
    }

    for(int i = 0; i < emitCount; ++i) {
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
        Math::AddScale2D(mat, scale);

        out.mat = mat.toMat3x2();

        out.col.r = Math::Lerp(p.startCol.r, p.endCol.r, prog);
        out.col.g = Math::Lerp(p.startCol.g, p.endCol.g, prog);
        out.col.b = Math::Lerp(p.startCol.b, p.endCol.b, prog);
        out.col.a = Math::Lerp(p.startCol.a, p.endCol.a, prog);

        float fadeProg = std::min(p.lifetime / colorConfig.fadeOut, (p.totalLifetime - p.lifetime) / colorConfig.fadeIn);
        fadeProg = std::min(fadeProg, 1.f);

        assert(fadeProg >= 0.f && fadeProg <= 1.f && "Invalid fade prog");

        out.col.a *= fadeProg;
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