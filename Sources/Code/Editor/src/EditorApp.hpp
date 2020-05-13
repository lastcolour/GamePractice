#ifndef __EDITOR_APP__
#define __EDITOR_APP__

#include "Application.hpp"
#include "Core/Buffer.hpp"

class EditorApp : public Application {
public:

    EditorApp();
    virtual ~EditorApp();

    bool initialize();
    void deinitiazlie();
    const char* getReflectModel();

protected:

    void buildModules(ModuleListT& modules) override;

private:

    Buffer reflectModelBuffer;
};

#endif /* __EDITOR_APP__ */