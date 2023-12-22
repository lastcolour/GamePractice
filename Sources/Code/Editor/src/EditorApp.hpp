#ifndef __EDITOR_APP__
#define __EDITOR_APP__

#include "Application.hpp"
#include "Platform/ETSurface.hpp"

#include <atomic>
#include <thread>

class TasksRunner;

class EditorApp : public Application {
public:

    EditorApp();
    virtual ~EditorApp();

    bool initialize();
    void deinitiazlie();
    Memory::Buffer getReflectModel();
    Memory::Buffer getRegisteredEntityLogics();
    EntityId loadEntityFromFile(const char* entityName);
    EntityId loadEntityFromData(const char* entityName, const char* entityData);
    void unloadEntity(EntityId entityId);
    EntityId getEntityChildEntityId(EntityId entityId, EntityChildId childId);
    const char* getEntityName(EntityId entityId);
    EntityLogicId addLogicToEntity(EntityId entityId, const char* logicName);
    void removeLogicFromEntity(EntityId entityId, EntityLogicId logicId);
    void drawFrame(void* out, int32_t width, int32_t height);
    EntityChildId addChilEntityToEntity(EntityId parentId, EntityId childId);
    void removeChildEntityFromEntity(EntityId parentId, EntityId childId);
    void addEntityLogicArrayElement(EntityId entityId, EntityLogicId logicId, Reflect::ClassValueId valueId);
    void setEntityLogicPolymorphObjectType(EntityId entityId, EntityLogicId logicId, Reflect::ClassValueId valueId, const char* newType);
    Memory::Buffer getEntityLogicData(EntityId entityId, EntityLogicId logicId, Reflect::ClassValueId valueId);
    void setEntityLogicData(EntityId entityId, EntityLogicId logicId, Reflect::ClassValueId valueId, Memory::Buffer& buffer);
    EntityChildId createChildEntity(EntityId entityId, const char* childName);
    void mouseInputEvent(EActionType actionType, const Vec2i& pos);
    void setTimeScale(float timeScale);
    void enableGameUpdate(bool flag);
    void unloadAll();
    bool renameEntity(EntityId entityId, const char* newName);
    void setFocusEntity(EntityId entityId);

protected:

    void buildModules(ModuleListT& modules) override;
    void buildTasksRunner();

private:

    std::atomic<int> tickCount;
    std::atomic<int> asyncTickingTasks;
    std::atomic<bool> canTickAsync;
    bool updateGame;

private:

    struct AsyncTickLock {
        AsyncTickLock(EditorApp& inApp) :
            app(inApp) {
            app.canTickAsync.store(false);
            while(app.asyncTickingTasks.load() != 0) {
                std::this_thread::yield();
            }
        }
        ~AsyncTickLock() {
            app.canTickAsync.store(true);
        }

    private:

        EditorApp& app;
    };

};

#endif /* __EDITOR_APP__ */