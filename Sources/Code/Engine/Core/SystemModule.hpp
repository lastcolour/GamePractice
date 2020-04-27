#ifndef __SYSTEM_MODULE_HPP__
#define __SYSTEM_MODULE_HPP__

#include "Core/Core.hpp"
#include "Core/ModuleConfig.hpp"
#include "Core/SystemLogicContainer.hpp"

#include <memory>
#include <string>

class SystemModule {
public:

    using LogicsContainerPtrT = std::unique_ptr<SystemLogicContainerBase>;
    using ConfigsPtrT = std::unique_ptr<ModuleConfigBase>;

public:

    explicit SystemModule(const char* moduleName);
    virtual ~SystemModule();

    bool init();

protected:

    virtual ConfigsPtrT getSystemConfigs() const = 0;
    virtual LogicsContainerPtrT getSystemLogics() const = 0;

private:

    SystemModule() = delete;
    SystemModule(const SystemModule&) = delete;
    SystemModule& operator=(const SystemModule&) = delete;

private:

    std::string name;
    LogicsContainerPtrT logicsContainer;
    ConfigsPtrT configs;
};

#endif /* __SYSTEM_MODULE_HPP__ */