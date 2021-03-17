#ifndef __SYSTEM_MODULE_HPP__
#define __SYSTEM_MODULE_HPP__

#include "Core/SystemLogicContainer.hpp"
#include "Core/JSONNode.hpp"

class EntityLogicsRegister;

class SystemModule {
public:

    using LogicsContainerPtrT = std::unique_ptr<SystemLogicContainerBase>;

public:

    explicit SystemModule(const char* moduleName);
    virtual ~SystemModule();

    bool init();

protected:

    virtual LogicsContainerPtrT createSystemLogics() const = 0;
    virtual void createSystemConfigs() const = 0;
    virtual void registerEntityLogics(EntityLogicsRegister& logicsRegister) const = 0;

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