#ifndef __MODULE_SHARED_MANAGER_HPP__
#define __MODULE_SHARED_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETApplication.hpp"
#include "Reflect/ClassInstance.hpp"

#include <unordered_map>

class ModuleSharedManager : public SystemLogic,
    public ETNode<ETModuleSharedManager> {
public:

    ModuleSharedManager();
    virtual ~ModuleSharedManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETModuleSharedManager
    void ET_registerShared(ClassInstance& instance) override;
    const void* ET_getShared(TypeId typeId) override;

private:

    std::unordered_map<TypeId, ClassInstance> sharedObjects;
};

#endif /* __MODULE_SHARED_MANAGER_HPP__ */