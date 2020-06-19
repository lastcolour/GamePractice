#ifndef __EDIT_ENTITY_TRACKER_HPP__
#define __EDIT_ENTITY_TRACKER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Render/ETRenderCamera.hpp"
#include "ETEditorInterfaces.hpp"

#include <unordered_set>

class EditEntityTracker : public SystemLogic,
    public ETNode<ETRenderCameraEvents>,
    public ETNode<ETEditEntityTracker> {
public:

    using EntittiesSetT = std::unordered_set<EntityId>;

public:

    EditEntityTracker();
    virtual ~EditEntityTracker();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRenderCameraEvents
    void ET_onRenderPortResized() override;

    // ETEditEntityTracker
    void ET_startTrackingEntity(EntityId entityId) override;
    void ET_stopTrackingEntity(EntityId entityId) override;

private:

    EntittiesSetT entities;
};

#endif /* __EDIT_ENTITY_TRACKER_HPP__ */