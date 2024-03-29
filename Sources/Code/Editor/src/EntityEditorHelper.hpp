#ifndef __EDIT_ENTITY_TRACKER_HPP__
#define __EDIT_ENTITY_TRACKER_HPP__

#include "Core/SystemLogic.hpp"
#include "ETEditorInterfaces.hpp"
#include "Render/ETDebugRender.hpp"

class EntityEditorHelper : public SystemLogic,
    public ETNode<ETEntityEditorHelper>,
    public ETNode<ETDebugInfoProvider> {
public:

    EntityEditorHelper();
    virtual ~EntityEditorHelper();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETEntityEditorHelper
    void ET_setFocusEntity(EntityId newFocusEntId) override;

    // ETDebugInfoProvider
    void ET_drawDebugInfo(DebugInfoDrawer& dd) override;

private:

    EntityId focusEntId;
};

#endif /* __EDIT_ENTITY_TRACKER_HPP__ */