#ifndef __EDITOR_APP__
#define __EDITOR_APP__

#include "Application.hpp"
#include "Core/Buffer.hpp"
#include "Core/Core.hpp"

class EditorApp : public Application {
public:

    EditorApp();
    virtual ~EditorApp();

    bool initialize();
    void deinitiazlie();
    const char* getReflectModel();
    EntityId loadEntity(const char* entityName);
    void unloadEntity(EntityId entityId);

protected:

    void buildModules(ModuleListT& modules) override;

private:

    Buffer reflectModelBuffer;
    EntityId centralEntityId;
};

#endif /* __EDITOR_APP__ */