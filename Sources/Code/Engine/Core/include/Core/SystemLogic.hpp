#ifndef __SYSTEM_LOGIC_HPP__
#define __SYSTEM_LOGIC_HPP__

#include "Core/Core.hpp"

class SystemLogic {
public:

    SystemLogic();
    virtual ~SystemLogic() = default;

    virtual bool init() = 0;
    virtual void deinit() = 0;

    EntityId getEntityId() const { return entityId; }

private:

    SystemLogic(const SystemLogic&) = delete;
    SystemLogic& operator=(const SystemLogic&) = delete;

private:

    EntityId entityId;
};

#endif /* __SYSTEM_LOGIC_HPP__ */