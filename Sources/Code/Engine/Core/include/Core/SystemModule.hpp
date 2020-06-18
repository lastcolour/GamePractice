#ifndef __SYSTEM_MODULE_HPP__
#define __SYSTEM_MODULE_HPP__

#include "Core/Core.hpp"
#include "Core/SystemLogicContainer.hpp"
#include "Core/JSONNode.hpp"

class EntityLogicsRegister;
class ReflectContext;

class SystemModule {
public:

    using LogicsContainerPtrT = std::unique_ptr<SystemLogicContainerBase>;

public:

    explicit SystemModule(const char* moduleName);
    virtual ~SystemModule();

    bool init();

protected:

    virtual LogicsContainerPtrT createSystemLogics() const = 0;
    virtual void reflectSystemConfigs(ReflectContext& ctx) const = 0;
    virtual void registerEntityLogics(EntityLogicsRegister& logicsRegister) const = 0;

protected:

    virtual bool serializeConfigs();
    virtual JSONNode loadModuleConfigs();

private:

    SystemModule() = delete;
    SystemModule(const SystemModule&) = delete;
    SystemModule& operator=(const SystemModule&) = delete;

private:

    std::string name;
    LogicsContainerPtrT logicsContainer;
    bool isInitialized;
};

#endif /* __SYSTEM_MODULE_HPP__ */