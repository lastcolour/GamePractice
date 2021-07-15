#include "Particles/EmitterParticles.hpp"
#include "Math/MatrixTransform.hpp"
#include "Render/ETParticlesSystem.hpp"
#include "Particles/ParticlesEmittersPool.hpp"
#include "RenderUtils.hpp"
#include "Core/Utils.hpp"
#include "Nodes/ETRenderNodeManager.hpp"

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

void simulateSimpleField(const GravityField& field, const Transform& tm, int activeCount, std::vector<Particle>& particles) {
    Vec2 fieldCenter = field.offset + Vec2(tm.pt.x, tm.pt.y);

    float radSq = field.radius * field.radius;
    float fieldAngle = Math::Deg2Rad(field.dir);
    Vec2 dtAcc = Vec2(cos(fieldAngle) * tm.scale.x, sin(fieldAngle) * tm.scale.y) * field.value;
    for(int i = 0; i < activeCount; ++i) {
        auto& p = particles[i];
        Vec2 dir = p.pt - fieldCenter;
        if(field.radius > 0.f && dir.lenghtSq() > radSq) {
            continue;
        }
        p.acc += dtAcc;
    }
}

void simulateVortexField(const GravityField& field, const Transform& tm, int activeCount, std::vector<Particle>& particles) {
    Vec2 fieldCenter = field.offset + Vec2(tm.pt.x, tm.pt.y);

    float radSq = field.radius * field.radius;
    float fieldAngle = Math::Deg2Rad(field.dir);
    for(int i = 0; i < activeCount; ++i) {
        auto& p = particles[i];
        Vec2 dir = fieldCenter - p.pt;
        if(field.radius > 0.f && dir.lenghtSq() > radSq) {
            continue;
        }
        dir.normalize();
        dir.scale(Vec2(tm.scale.x, tm.scale.y));
        dir = Math::RotateVec2D(dir, fieldAngle);
        p.acc += dir * field.value;
    }
}

bool triggerSubEmitters(const Particle* p, SubEmitterTriggerEvent event,
    const SimulationConfig& simConf, const EmitRequest& emitReq, SubEmitterTriggerEvent flags) {

    if(!EnumFlagsIntersect(event, flags)) {
        return false;
    }

    int pId = -1;
    Transform tm;
    tm = emitReq.tm;
    if(p) {
        pId = p->id;
        tm.pt = Vec3(p->pt.x, p->pt.y, 0.f);
        tm.quat.setAxisAngle(Vec3(0.f, 0.f, 1.f), p->rot);
    }

    for(auto& sub : simConf.subEmittorsConfig.subEmitters) {
        if(!sub.entId.isValid()) {
            continue;
        }
        if(sub.event == SubEmitterTriggerEvent::OnParticleLive) {
            if(event == SubEmitterTriggerEvent::OnParticleSpawn) {
                ET_SendEvent(sub.entId, &ETParticlesSystem::ET_spawnSubEmitter, pId, tm);
            } else if(event == SubEmitterTriggerEvent::OnParticleLive) {
                ET_SendEvent(sub.entId, &ETParticlesSystem::ET_updateSubEmitter, pId, tm);
            } else if(event == SubEmitterTriggerEvent::OnParticleDeath) {
                ET_SendEvent(sub.entId, &ETParticlesSystem::ET_stopSubEmitter, pId);
            }
        } else if(sub.event == event) {
            ET_SendEvent(sub.entId, &ETParticlesSystem::ET_spawnSubEmitter, pId, tm);
        }
    }

    return true;
}

SubEmitterTriggerEvent caclSubEmitterFlags(const SimulationConfig& simConf) {
    SubEmitterTriggerEvent flags = SubEmitterTriggerEvent::None;
    for(auto& sub : simConf.subEmittorsConfig.subEmitters) {
        if(!sub.entId.isValid()) {
            continue;
        }
        assert(sub.event != SubEmitterTriggerEvent::None && "Invalid sub event");
        flags = EnumFlagsUnite(flags, sub.event);
    }
    return flags;
}

int getNewParticlesAmount(const SimulationConfig& simConf, float dt) {
    auto maxAmount = simConf.emission.lifetime * simConf.emission.emissionRate;
    auto emitCount = simConf.emission.emissionRate * dt;
    if(maxAmount < 1.f) {
        emitCount = std::min(1.f, emitCount);
    } else {
        emitCount = std::min(maxAmount, emitCount);
    }
    return static_cast<int>(emitCount);
}

} // namespace

EmitterParticles::EmitterParticles(ParticlesEmittersPool& emittersPool) :
    pool(emittersPool),
    simConf(emittersPool.getSimConfig()),
    duration(0.f),
    emitFracTime(0.f),
    activeCount(0),
    floatGen(0.f, 1.f),
    spawnedCount(0),
    emissionState(EmissionState::Finished),
    subEmitterFlags(SubEmitterTriggerEvent::None) {
}

EmitterParticles::~EmitterParticles() {
    if(emissionState == EmissionState::Finished) {
        return;
    }
    for(int i = 0; i < activeCount; ++i) {
        const auto& p = particles[i];
        if(!triggerSubEmitters(&p, SubEmitterTriggerEvent::OnParticleDeath,
            simConf, emitReq, subEmitterFlags)) {
            break;
        }
    }
}

void EmitterParticles::removeOld(float dt) {
    int j = 0;
    for(int i = 0; i < activeCount; ++i) {
        auto& p = particles[i];
        p.lifetime -= dt;
        if(p.lifetime > 0.f) {
            particles[j] = particles[i];
            instaceData[j] = instaceData[i];
            ++j;
        } else {
            triggerSubEmitters(&p, SubEmitterTriggerEvent::OnParticleDeath,
                simConf, emitReq, subEmitterFlags);
        }
    }
    auto removedCount = activeCount - j;
    pool.removeParticles(removedCount);
    activeCount = j;
}

void EmitterParticles::spawnNewParticle(Particle& p) {
    Vec2 startPt(0.f);

    Vec2 emitterVal = simConf.emission.emitterVal;
    switch(simConf.emission.emitterType) {
        case EmitterType::Sphere: {
            float shpereR = emitterVal.x;
            float t = getRandomFloatInRange(floatGen, 0.f, 1.f);
            float radius = Math::Lerp(shpereR * simConf.emission.thickness, shpereR, t);
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
                startPt.y = tSign * Math::Lerp(emitterVal.y * simConf.emission.thickness, emitterVal.y, t);
            } else {
                startPt.y = Math::Lerp(-emitterVal.y, emitterVal.y, -t); 
                t = getRandomFloatInRange(floatGen, -1.f, 1.f);
                float tSign = 1.f;
                if(t < 0.f) {
                    tSign = -1.f;
                    t = -t; 
                }
                startPt.x = tSign * Math::Lerp(emitterVal.x * simConf.emission.thickness, emitterVal.x, t);
            }
            break;
        }
        default: {
            assert(false && "Invalid emitter type");
        }
    }

    p.pt = startPt;
    p.lifetime = getRandomLifetime(floatGen, simConf.emission.lifetime, simConf.emission.lifetimeVar);
    p.totalLifetime = p.lifetime;

    Vec2 moveDir = getRandomDir(floatGen, simConf.emission.direction, simConf.emission.directionVar);

    p.speed = moveDir * getRandomSpeed(floatGen, simConf.movement.speed, simConf.movement.speedVar);

    p.startScale = getRandomScale(floatGen, simConf.sizeConfig.startScale, simConf.sizeConfig.startScaleVar);
    p.endScale = getRandomScale(floatGen, simConf.sizeConfig.endScale, simConf.sizeConfig.endScaleVar);

    p.rot = getRandomRotation(floatGen, simConf.movement.initRotation, simConf.movement.initRotationVar);
    p.rotSpeed = getRandomRotation(floatGen, simConf.movement.rotationSpeed, simConf.movement.rotationSpeedVar);

    p.startCol = getRandomColor(floatGen, simConf.color.startCol.getColorF(), simConf.color.startColVar);
    p.endCol = getRandomColor(floatGen, simConf.color.endCol.getColorF(), simConf.color.endColVar);

    p.acc = Vec2(0.f);

    p.id = generateParticleId();

    const auto& tm = emitReq.tm;
    if(simConf.emission.emitterSpace == EmitterSpace::World) {
        p.pt.x *= tm.scale.x;
        p.pt.y *= tm.scale.y;

        p.pt += Vec2(tm.pt.x, tm.pt.y);
        p.startScale.x *= tm.scale.x;
        p.startScale.y *= tm.scale.y;

        p.endScale.x *= tm.scale.x;
        p.endScale.y *= tm.scale.y;

        p.speed.scale(tm.scale.x, tm.scale.y);

        Vec3 speed = tm.quat * Vec3(p.speed, 0.f);
        p.speed = Vec2(speed.x, speed.y);
    }
}

int EmitterParticles::generateParticleId() {
    spawnedCount += 1;
    return spawnedCount;
}

void EmitterParticles::emitNew(float dt) {
    emitFracTime += dt;
    auto emitCount = getNewParticlesAmount(simConf, emitFracTime);
    emitCount = pool.addParticles(emitCount);
    if(emitCount == 0) {
        return;
    }

    emitFracTime -= emitCount / simConf.emission.emissionRate;

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
        spawnNewParticle(p);
        triggerSubEmitters(&p, SubEmitterTriggerEvent::OnParticleSpawn,
            simConf, emitReq, subEmitterFlags);
    }

    activeCount += emitCount;
}

void EmitterParticles::moveAlive(float dt) {
    Transform tm;
    if(simConf.emission.emitterSpace == EmitterSpace::World) {
        tm = emitReq.tm;
    }

    simulateGravities(tm, dt);

    for(int i = 0; i < activeCount; ++i) {
        auto& p = particles[i];

        p.speed += p.acc * dt;
        p.pt += p.speed * dt;
        p.acc = Vec2(0.f);

        p.rotSpeed += simConf.movement.rotationAcc * dt;
        p.rot += p.rotSpeed * dt;
    }

    updateIntacesData(dt);

    for(int i = 0; i < activeCount; ++i) {
        auto& p = particles[i];
        if(!triggerSubEmitters(&p, SubEmitterTriggerEvent::OnParticleLive,
            simConf, emitReq, subEmitterFlags)) {
            break;
        }
    }
}

void EmitterParticles::updateIntacesData(float dt) {
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

        float fadeProg = std::min(p.lifetime / simConf.color.fadeOut, (p.totalLifetime - p.lifetime) / simConf.color.fadeIn);
        fadeProg = Math::Clamp(fadeProg, 0.f, 1.f);

        out.col.a *= fadeProg;
    }
}

void EmitterParticles::simulateGravities(const Transform& tm, float dt) {
    for(auto& field : simConf.gravity.fields) {
        switch(field.type) {
            case GravityType::Simple: {
                simulateSimpleField(field, tm, activeCount, particles);
                break;
            }
            case GravityType::Vortex: {
                simulateVortexField(field, tm, activeCount, particles);
                break;
            }
        }
    }
}

void EmitterParticles::updateState(float dt) {
    switch(emissionState) {
        case EmissionState::StartDelay: {
            duration += dt;
            if(duration < simConf.emission.startDelay) {
                break;
            }
            triggerSubEmitters(nullptr, SubEmitterTriggerEvent::OnStart, simConf,
                emitReq, subEmitterFlags);
            duration = 0.f;
            emitFracTime = 0.f;
            emissionState = EmissionState::Alive;
            break;
        }
        case EmissionState::Alive: {
            if(simConf.emission.duration < 0.f) {
                break;
            }
            duration += dt;
            if(duration >= simConf.emission.duration) {
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
                triggerSubEmitters(nullptr, SubEmitterTriggerEvent::OnEnd, simConf,
                    emitReq, subEmitterFlags);
                if(simConf.emission.loop) {
                    start(emitReq);
                } else {
                    emitReq.reset();
                }
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

void EmitterParticles::simulate(const Transform& systemTm, float dt) {
    updateState(dt);
    if(emissionState == EmissionState::Finished) {
        return;
    }
    updateTransform(systemTm);
    if(activeCount > 0) {
        removeOld(dt);
    }
    if(emissionState == EmissionState::Alive) {
        emitNew(dt);
    }
    if(activeCount > 0) {
        moveAlive(dt);
    }
}

void EmitterParticles::start(const EmitRequest& emitRequest) {
    duration = 0.f;
    emitFracTime = 0.f;
    activeCount = 0;
    emissionState = EmissionState::StartDelay;
    emitReq = emitRequest;
    subEmitterFlags = caclSubEmitterFlags(simConf);
}

void EmitterParticles::stop() {
    duration = 0.f;
    emitFracTime = 0.f;
    activeCount = 0;
    emissionState = EmissionState::Finished;
    emitReq.reset();
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
    if(simConf.emission.emitterSpace != EmitterSpace::World) {
        resMat = emitReq.tm.toMat4();
    }
    return resMat;
}

const EmitRequest& EmitterParticles::getEmitRequest() const {
    return emitReq;
}

void EmitterParticles::updateSubEmitterTm(const Transform& newTm) {
    assert(emitReq.tmTrackType == EParticleTMTrackType::None && "Can't update emitter pos");
    emitReq.tm = newTm;
}

void EmitterParticles::updateTransform(const Transform& systemTm) {
    switch(emitReq.tmTrackType) {
        case EParticleTMTrackType::None: {
            break;
        }
        case EParticleTMTrackType::System: {
            emitReq.tm = systemTm;
            break;
        }
        case EParticleTMTrackType::Entity: {
            ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [req=&emitReq](){
                ET_SendEventReturn(req->tm, req->trackEntId, &ETEntity::ET_getTransform);
            });
            break;
        }
        default: {
            assert(false && "Invalid track tm type");
        }
    }
}