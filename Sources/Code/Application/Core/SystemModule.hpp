#ifndef __SYSTEM_MODULE_HPP__
#define __SYSTEM_MODULE_HPP__

#include "Core/Core.hpp"
#include "Core/Utils.hpp"

#include <memory>
#include <string>

class SystemModule;
class SystemLogic;

class SystemLogicContainerBase {
public:
    virtual ~SystemLogicContainerBase() = default;
    virtual bool init() = 0;
    virtual void deinit() = 0;
    virtual bool empty() const = 0;
    virtual void setParentModule(SystemModule& module) = 0;
};

template<typename ... ArgsT>
class SystemLogicContainer : public SystemLogicContainerBase {

    typedef std::tuple<ArgsT...> LogicsTupleT;

public:

    SystemLogicContainer() = default;
    virtual ~SystemLogicContainer() = default;

    bool init() override {
        std::vector<bool> results;
        ApplyTupleResult(logicsTuple, [](auto& logic){
            return logic.init();
        }, results);
        for(const auto& res : results) {
            if(!res) {
                return false;
            }
        }
        return true;
    }

    void setParentModule(SystemModule& module) override {
        ApplyTuple(logicsTuple, [this, &module](auto& logic){
            this->setParentModule(static_cast<SystemLogic&>(logic), module);
        });
    }

    void deinit() override {
        ApplyTuple(logicsTuple, [](auto& logic){
            logic.deinit();
        });
    }

    bool empty() const override {
        return std::tuple_size<LogicsTupleT>::value == 0;
    }

private:

    void setParentModule(SystemLogic& logic, SystemModule& module) {
        logic.setParentModule(module);
    }

private:

    LogicsTupleT logicsTuple;
};

class SystemModule {
public:

    typedef std::unique_ptr<SystemLogicContainerBase> LogicsContainerPtrT;

public:

    SystemModule(const std::string& moduleName);
    virtual ~SystemModule() = default;

    bool init();

    EntityId getEntityId() const;

protected:

    virtual LogicsContainerPtrT getSystemLogics() const = 0;

private:

    SystemModule() = delete;
    SystemModule(const SystemModule&) = delete;
    SystemModule& operator=(const SystemModule&) = delete;

private:

    std::string name;
    LogicsContainerPtrT logicsContainer;
    EntityId moduleId;
};

#endif /* __SYSTEM_MODULE_HPP__ */