#ifndef __SYSTEM_LOGIC_HPP__
#define __SYSTEM_LOGIC_HPP__

#include "Core/Core.hpp"

class SystemModule;

class SystemLogic {

    template<typename ... ArgsT>
    friend class SystemLogicContainer;

public:

    SystemLogic();
    virtual ~SystemLogic() = default;

    virtual bool init() = 0;
    virtual void deinit() = 0;

    EntityId getEntityId() const;

private:

    SystemLogic(const SystemLogic&) = delete;
    SystemLogic& operator=(const SystemLogic&) = delete;

private:

    void setParentModule(SystemModule& module);

private:

    SystemModule* module;
};

#endif /* __SYSTEM_LOGIC_HPP__ */