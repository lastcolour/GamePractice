#ifndef __ET_EDITOR_INTERFACES_HPP__
#define __ET_EDITOR_INTERFACES_HPP__

#include "Core/Core.hpp"

struct ETEditEntityTracker {
    virtual ~ETEditEntityTracker() = default;
    virtual void ET_startTrackingEntity(EntityId entityId) = 0;
    virtual void ET_stopTrackingEntity(EntityId entityId) = 0;
};

#endif /* __ET_EDITOR_INTERFACES_HPP__ */