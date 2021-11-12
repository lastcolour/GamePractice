#ifndef __SYSTEM_LOGIC_CONTAINER_HPP__
#define __SYSTEM_LOGIC_CONTAINER_HPP__

class SystemLogicContainerBase {
public:
    virtual ~SystemLogicContainerBase() = default;
    virtual bool init() = 0;
    virtual void deinit() = 0;
    virtual bool empty() const = 0;
};

template<typename ... ArgsT>
class SystemLogicContainer : public SystemLogicContainerBase {

    using LogicsTupleT = std::tuple<ArgsT...>;

public:

    SystemLogicContainer() = default;
    virtual ~SystemLogicContainer() = default;

    bool init() override {
        std::vector<bool> results;
        Core::ApplyTupleResult(logicsTuple, [](auto& logic){
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
        Core::ApplyTupleReverse(logicsTuple, [](auto& logic){
            logic.deinit();
        });
    }

    bool empty() const override {
        return std::tuple_size<LogicsTupleT>::value == 0;
    }

private:

    LogicsTupleT logicsTuple;
};

#endif /* __SYSTEM_LOGIC_CONTAINER_HPP__ */