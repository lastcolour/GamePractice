#ifndef __SYSTEM_LOGIC_HPP__
#define __SYSTEM_LOGIC_HPP__

#include "Core/Core.hpp"

class SystemLogic {
public:

    SystemLogic();
    virtual ~SystemLogic();

    bool init();
    bool shouldRun();
    void update(float dt);
    EntityId getEntityId() const;

protected:

    virtual bool onInit() = 0;
    virtual bool onShouldRun() { return true; }
    virtual void onUpdate(float dt) { (void)dt; }

private:

    EntityId entityId;
};

#endif /* __SYSTEM_LOGIC_HPP__ */