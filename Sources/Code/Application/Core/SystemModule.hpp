#ifndef __SYSTEM_MODULE_HPP__
#define __SYSTEM_MODULE_HPP__

#include "Core/Utils.hpp"

#include <memory>
#include <string>

class SystemLogicContainerBase {
public:
    virtual ~SystemLogicContainerBase() = default;
    virtual bool init() = 0;
    virtual void deinit() = 0;
    virtual bool empty() const = 0;
};

template<typename ... ArgsT>
class SystemLogicContainer : public SystemLogicContainerBase {
public:

    SystemLogicContainer() = default;
    virtual ~SystemLogicContainer() = default;

    bool init() override {
        std::vector<bool> results;
        ApplyTupleResult(logicsTuple, [](auto& logic) {
            return logic.init();
        }, results);
        for(const auto& res : results) {
            if(!res) {
                return false;
            }
        }
        return true;
    }

    void deinit() override {
        ApplyTuple(logicsTuple, [](auto& logic) {
            logic.deinit();
        });
    }

    bool empty() const override {
        return std::tuple_size<LogicsTupleT>::value == 0;
    }

private:

    typedef std::tuple<ArgsT...> LogicsTupleT;
    LogicsTupleT logicsTuple;
};

class SystemModule {
public:

    typedef std::unique_ptr<SystemLogicContainerBase> LogicsContainerPtrT;

public:

    SystemModule(const std::string& moduleName);
    virtual ~SystemModule() = default;

    bool init();

protected:

    virtual LogicsContainerPtrT getSystemLogics() const = 0;

private:

    std::string name;
    LogicsContainerPtrT logicsContainer;
};

#endif /* __SYSTEM_MODULE_HPP__ */