#include "Particles/EmitterParticles.hpp"
#include "Logics/RenderAuxFunctions.hpp"
#include "Math/MatrixTransform.hpp"
#include "Core/ETPrimitives.hpp"
#include "Render/ETParticlesSystem.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>

namespace {

const float DEF_HEAT_DURATION = 10.f;

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

void simulateSimpleField(const GravityField& field, int activeCount, std::vector<Particle>& particles, float dt) {
    float radSq = field.radius * field.radius;
    float fieldAngle = Math::Deg2Rad(field.dir);
    Vec2 dtAcc = Vec2(cos(fieldAngle), sin(fieldAngle)) * field.value * dt;
    for(int i = 0; i < activeCount; ++i) {
        auto& p = particles[i];
        Vec2 dir = p.pt - field.offset;
        if(field.radius > 0.f && dir.getLenghtSq() > radSq) {
            continue;
        }
        p.acc += dtAcc;
    }
}

void simulateVortexField(const GravityField& field, int activeCount, std::vector<Particle>& particles, float dt) {
    float radSq = field.radius * field.radius;
    float fieldAngle = Math::Deg2Rad(field.dir);
    for(int i = 0; i < activeCount; ++i) {
        auto& p = particles[i];
        Vec2 dir = field.offset - p.pt;
        if(field.radius > 0.f && dir.getLenghtSq() > radSq) {
            continue;
        }
        dir.normalize();
        dir = Math::RotateVec2D(dir, fieldAngle);
        p.acc += dir * field.value * dt;
    }
}

void triggerSubEmitters(int particleId, const Vec2& pt, SubEmitterTriggerEvent event, const SimulationConfig& simConfig) {
    for(auto& sub : simConfig.subEmittorsConfig.subEmitters) {
        if(!sub.entId.isValid()) {
            continue;
        }
        if(sub.event == SubEmitterTriggerEvent::OnParticleLive) {
            if(event == SubEmitterTriggerEvent::OnParticleSpawn) {
                ET_SendEvent(sub.entId, &ETParticlesSystem::ET_spawnSubEmitter, particleId, pt);
            } else if(event == SubEmitterTriggerEvent::OnParticleLive) {
                ET_SendEvent(sub.entId, &ETParticlesSystem::ET_updateSubEmitter, particleId, pt);
            } else if(event == SubEmitterTriggerEvent::OnParticleDeath) {
                ET_SendEvent(sub.entId, &ETParticlesSystem::ET_stopSubEmitter, particleId);
            }
        } else if(sub.event == event) {
            ET_SendEvent(sub.entId, &ETParticlesSystem::ET_spawnSubEmitter, particleId, pt);
        }
    }
}

} // namespace

EmitterParticles::EmitterParticles() :
    duration(0.f),
    emitFracTime(0.f),
    activeCount(0),
    floatGen(0.f, 1.f),
    spawnedCount(0),
    emissionState(EmissionState::Finished),
    wroldTmSpace(false) {
}

void EmitterParticles::removeOld(const SimulationConfig& simConfig, float dt) {
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
        } else {
            triggerSubEmitters(p.id, p.pt, SubEmitterTriggerEvent::OnParticleDeath, simConfig);
        }
    }
    activeCount = j;
}

void EmitterParticles::spawnNewParticle(const SimulationConfig& simConfig, Particle& p) {
    Vec2 startPt(0.f);

    Vec2 emitterVal = simConfig.emission.emitterVal;
    switch(simConfig.emission.emitterType) {
        case EmitterType::Sphere: {
            float shpereR = emitterVal.x;
            float t = getRandomFloatInRange(floatGen, 0.f, 1.f);
            float radius = Math::Lerp(shpereR * simConfig.emission.thickness, shpereR, t);
            float angel = getRandomFloatInRange(floatGen, 0.f, 2.f * Math::PI);

            startPt.x = radius * cos(angel);
            startPt.y = radius * sin(angel);
            break;
        }
        case EmitterType::Box: {
            float t = getRandomFloatInRange(floatGen, -1.f, 1.f);
            if(t > 0.f) {
                startPt.x = Math::Lerp(-emitterVal.x, emitterVal.x, t);
                t = getRandomFloatInRange(floatGen, -1.f, 1.f);
                float tSign = 1.f;
                if(t < 0.f) {
                    tSign = -1.f;
                    t = -t; 
                }
                startPt.y = tSign * Math::Lerp(emitterVal.y * simConfig.emission.thickness, emitterVal.y, t);
            } else {
                startPt.y = Math::Lerp(-emitterVal.y, emitterVal.y, -t); 
                t = getRandomFloatInRange(floatGen, -1.f, 1.f);
                float tSign = 1.f;
                if(t < 0.f) {
                    tSign = -1.f;
                    t = -t; 
                }
                startPt.x = tSign * Math::Lerp(emitterVal.x * simConfig.emission.thickness, emitterVal.x, t);
            }
            break;
        }
        default: {
            assert(false && "Invalid emitter type");
        }
    }

    p.pt = startPt;
    p.lifetime = getRandomLifetime(floatGen, simConfig.emission.lifetime, simConfig.emission.lifetimeVar);
    p.totalLifetime = p.lifetime;

    Vec2 moveDir = getRandomDir(floatGen, simConfig.emission.direction, simConfig.emission.directionVar);

    p.speed = moveDir * getRandomSpeed(floatGen, simConfig.movement.speed, simConfig.movement.speedVar);

    p.startScale = getRandomScale(floatGen, simConfig.sizeConfig.startScale, simConfig.sizeConfig.startScaleVar);
    if(simConfig.sizeConfig.fixedRatio) {
        p.endScale = p.startScale * getRandomFloatInRange(floatGen, simConfig.sizeConfig.endScale.x, simConfig.sizeConfig.endScale.y);
    } else {
        p.endScale = getRandomScale(floatGen, simConfig.sizeConfig.endScale, simConfig.sizeConfig.endScaleVar);
    }

    p.rot = getRandomRotation(floatGen, simConfig.movement.initRotation, simConfig.movement.initRotationVar);
    p.rotSpeed = getRandomRotation(floatGen, simConfig.movement.rotationSpeed, simConfig.movement.rotationSpeedVar);

    p.startCol = getRandomColor(floatGen, simConfig.color.startCol.getColorF(), simConfig.color.startColVar);
    p.endCol = getRandomColor(floatGen, simConfig.color.endCol.getColorF(), simConfig.color.endColVar);

    p.acc = Vec2(0.f);

    p.id = generateParticleId();

    const auto& tm = emitReq.tm;
    if(wroldTmSpace) {
        p.pt.x *= tm.scale.x;
        p.pt.y *= tm.scale.y;

        p.pt += Vec2(tm.pt.x, tm.pt.y);
        p.startScale.x *= tm.scale.x;
        p.startScale.y *= tm.scale.y;

        p.endScale.x *= tm.scale.x;
        p.endScale.y *= tm.scale.y;

        Vec3 speed = tm.quat * Vec3(p.speed, 0.f);
        p.speed = Vec2(speed.x, speed.y);
    }
}

int EmitterParticles::generateParticleId() {
    spawnedCount += 1;
    return spawnedCount;
}

void EmitterParticles::emitNew(const SimulationConfig& simConfig, float dt) {
    emitFracTime += dt;

    if(!activeCount && !simConfig.emission.heating) {
        emitFracTime = DEF_HEAT_DURATION;
    }

    if(simConfig.emission.duration > 0.f) {
        emitFracTime = std::min(emitFracTime, simConfig.emission.duration);
    }

    int emitCount = static_cast<int>(simConfig.emission.emissionRate * emitFracTime);
    emitCount = std::min(Render::MaxParticlessPerDraw, activeCount + emitCount) - activeCount;
    if(emitCount == 0) {
        return;
    }

    emitFracTime -= emitCount / simConfig.emission.emissionRate;

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
        spawnNewParticle(simConfig, p);
        triggerSubEmitters(p.id, p.pt, SubEmitterTriggerEvent::OnParticleSpawn, simConfig);
    }

    activeCount += emitCount;
    assert(activeCount <= Render::MaxParticlessPerDraw && "Too Many Particles");
}

void EmitterParticles::moveAlive(const SimulationConfig& simConfig, float dt) {
    for(int i = 0; i < activeCount; ++i) {
        auto& p = particles[i];
        auto& out = instaceData[i];

        p.speed += p.acc * dt;
        p.pt += p.speed * dt;
        p.acc = Vec2(0.f);

        p.rot += p.rotSpeed * dt;
        p.rotSpeed += simConfig.movement.rotationAcc * dt;

        triggerSubEmitters(p.id, p.pt, SubEmitterTriggerEvent::OnParticleLive, simConfig);
    }
}

void EmitterParticles::updateIntacesData(const SimulationConfig& simConfig, float dt) {
    for(int i = 0; i < activeCount; ++i) {
        auto& p = particles[i];
        auto& out = instaceData[i];

        auto prog = 1.f - p.lifetime / p.totalLifetime;

        Vec2 scale = Math::Lerp(p.startScale, p.endScale, prog);

        Mat3 mat(1.f);
        Math::AddTranslate2D(mat, p.pt);
        Math::AddRotate2D(mat, p.rot);
        Math::AddScale2D(mat, scale);

        out.mat = mat.toMat3x2();

        out.col.r = Math::Lerp(p.startCol.r, p.endCol.r, prog);
        out.col.g = Math::Lerp(p.startCol.g, p.endCol.g, prog);
        out.col.b = Math::Lerp(p.startCol.b, p.endCol.b, prog);
        out.col.a = Math::Lerp(p.startCol.a, p.endCol.a, prog);

        float fadeProg = std::min(p.lifetime / simConfig.color.fadeOut, (p.totalLifetime - p.lifetime) / simConfig.color.fadeIn);
        fadeProg = std::min(fadeProg, 1.f);

        assert(fadeProg >= 0.f && fadeProg <= 1.f && "Invalid fade prog");

        out.col.a *= fadeProg;
    }
}

void EmitterParticles::simulateGravities(const SimulationConfig& simConfig, float dt) {
    for(auto& field : simConfig.gravity.fields) {
        switch(field.type) {
            case GravityType::Simple: {
                simulateSimpleField(field, activeCount, particles, dt);
                break;
            }
            case GravityType::Vortex: {
                simulateVortexField(field, activeCount, particles, dt);
                break;
            }
        }
    }
}

void EmitterParticles::updateState(const SimulationConfig& simConfig, float dt) {
    switch(emissionState) {
        case EmissionState::StartDelay: {
            duration += dt;
            if(duration < simConfig.emission.startDelay) {
                break;
            }
            emissionState = EmissionState::Alive;
            triggerSubEmitters(-1, Vec2(emitReq.tm.pt.x, emitReq.tm.pt.y), SubEmitterTriggerEvent::OnStart, simConfig);
            spawnedCount = 0;
            duration = 0.f;
            emitFracTime = 0.f;
            duration -= simConfig.emission.startDelay;
            break;
        }
        case EmissionState::Alive: {
            if(simConfig.emission.duration < 0.f) {
                break;
            }
            duration += dt;
            if(duration > simConfig.emission.duration) {
                duration = 0.f;
                emitFracTime = 0.f;
                emissionState = EmissionState::WaitingAlive;
            }
            break;
        }
        case EmissionState::WaitingAlive: {
            if(activeCount == 0) {
                emissionState = EmissionState::Finished;
                activeCount = 0;
                triggerSubEmitters(-1, Vec2(emitReq.tm.pt.x, emitReq.tm.pt.y), SubEmitterTriggerEvent::OnEnd, simConfig);
            }
            break;
        }
        case EmissionState::Finished: {
            assert(false && "Can't update finished emitter");
            break;
        }
        default: {
            assert(false && "Invalid emitter life type");
        }
    }
}

void EmitterParticles::simulate(const SimulationConfig& simConfig, const Transform& systemTm, float dt) {
    if(emitReq.syncWithSystemTm) {
        emitReq.tm = systemTm;
    }
    updateState(simConfig, dt);
    if(emissionState == EmissionState::Finished) {
        return;
    }
    wroldTmSpace = simConfig.emission.emitterSpace == EmitterSpace::World;
    if(activeCount > 0) {
        removeOld(simConfig, dt);
    }
    if(activeCount > 0) {
        simulateGravities(simConfig, dt);
        moveAlive(simConfig, dt);
    }
    if(emissionState == EmissionState::Alive) {
        emitNew(simConfig, dt);
    }
    if(activeCount > 0) {
        updateIntacesData(simConfig, dt);
    }
}

void EmitterParticles::start(const EmitRequest& emitRequest) {
    emitReq = emitRequest;
    emissionState = EmissionState::StartDelay;
}

void EmitterParticles::stop() {
    spawnedCount = 0;
    duration = 0.f;
    emitFracTime = 0.f;
    activeCount = 0;
    emissionState = EmissionState::Finished;
}

void EmitterParticles::stopEmitting() {
    emissionState = EmissionState::WaitingAlive;
}

bool EmitterParticles::isEmitting() const {
    return emissionState == EmissionState::Alive;
}

bool EmitterParticles::hasAlive() const {
    return activeCount > 0;
}

bool EmitterParticles::isFinished() const {
    return emissionState == EmissionState::Finished;
}

Mat4 EmitterParticles::getTransformMat() const {
    Mat4 resMat(1.f);
    if(!wroldTmSpace) {
        resMat = emitReq.tm.toMat4();
    }
    return resMat;
}

int EmitterParticles::getRootParticleId() const {
    return emitReq.rootParticleId;
}

void EmitterParticles::updateEmitPos(const Vec2& newEmitPt) {
    assert(!emitReq.syncWithSystemTm && "Can't update emitter pos");
    emitReq.tm.pt = Vec3(newEmitPt, 0.f);
}