#ifndef __ET_TASKS_HPP__
#define __ET_TASKS_HPP__

struct ETRenderUpdateTask {
    virtual ~ETRenderUpdateTask() = default;
    virtual void ET_updateRender(float dt) = 0;
    virtual void ET_updateParticles(float dt) = 0;
    virtual void ET_syncWithGame() = 0;
};

struct ETInputUpdateTask {
    virtual ~ETInputUpdateTask() = default;
    virtual void ET_updateInput() = 0;
};

struct ETSoundUpdateTask {
    virtual ~ETSoundUpdateTask() = default;
    virtual void ET_updateSound() = 0;
};

struct ETAssetsUpdateTask {
    virtual ~ETAssetsUpdateTask() = default;
    virtual void ET_updateAssets(float dt) = 0;
};

struct ETEntitiesUpdateTask {
    virtual ~ETEntitiesUpdateTask() = default;
    virtual void ET_updateEntities() = 0;
};

#endif /* __ET_TASKS_HPP__ */