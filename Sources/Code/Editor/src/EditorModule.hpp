#ifndef __EDITOR_MODULE_HPP__
#define __EDITOR_MODULE_HPP__

#include "Core/SystemModule.hpp"

class EditorModule : public SystemModule {
public:

    EditorModule();
    virtual ~EditorModule();

protected:

    LogicsContainerPtrT createSystemLogics() const override;
    void reflectSystemConfigs(ReflectContext& ctx) const override;
    void registerEntityLogics(EntityLogicsRegister& logicsRegister) const override;
};

#endif /* __EDITOR_MODULE_HPP__ */