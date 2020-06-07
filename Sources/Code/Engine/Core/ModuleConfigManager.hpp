#ifndef __MODULE_CONFIG_MANAGER_HPP__
#define __MODULE_CONFIG_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Reflect/ClassInstance.hpp"

#include <unordered_map>

class ModuleConfigManager : public SystemLogic,
    public ETNode<ETModuleConfigManager> {
public:

    ModuleConfigManager();
    virtual ~ModuleConfigManager();
 
    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETModuleConfigManager
    void ET_registerConfig(ClassInstance& configInstance) override;
    void* ET_getConfig(TypeId configId) override;

private:

    std::unordered_map<TypeId, ClassInstance> configs;
};

#endif /* __MODULE_CONFIG_MANAGER_HPP__ */