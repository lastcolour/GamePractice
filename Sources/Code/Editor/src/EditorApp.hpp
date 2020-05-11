#ifndef __EDITOR_APP__
#define __EDITOR_APP__

#include "Application.hpp"

class EditorApp : public Application {
public:

    EditorApp();
    virtual ~EditorApp();

    bool initialize();
    void deinitiazlie();
    const char* getReflectModel() const;

protected:

    void buildModules(ModuleListT& modules) override;
};

#endif /* __EDITOR_APP__ */