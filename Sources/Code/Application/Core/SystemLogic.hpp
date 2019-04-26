#ifndef __SYSTEM_LOGIC_HPP__
#define __SYSTEM_LOGIC_HPP__

#include "Core/Core.hpp"

class SystemLogic {
public:
    virtual ~SystemLogic() = default;

    virtual bool init() = 0;
    virtual void deinit() = 0;

protected:

    EntityId getEntityId() const { return InvalidEntityId; }
};

#endif /* __SYSTEM_LOGIC_HPP__ */