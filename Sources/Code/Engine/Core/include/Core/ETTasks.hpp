#ifndef __ET_TASKS_HPP__
#define __ET_TASKS_HPP__

struct ETGameUpdateTask {
    virtual ~ETGameUpdateTask() = default;
    virtual void ET_updateGame() = 0;
};

struct ETRenderUpdateTask {
    virtual ~ETRenderUpdateTask() = default;
    virtual void ET_updateRender() = 0;
};

struct ETUIUpdateTask {
    virtual ~ETUIUpdateTask() = default;
    virtual void ET_updateUI() = 0;
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
    virtual void ET_updateAssets() = 0;
};

struct ETEntitiesUpdateTask {
    virtual ~ETEntitiesUpdateTask() = default;
    virtual void ET_updateEntities() = 0;
};

#endif /* __ET_TASKS_HPP__ */