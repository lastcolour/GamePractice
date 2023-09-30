#ifndef __UI_REALIGN_MANAGER_HPP__
#define __UI_REALIGN_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "UI/ETUIView.hpp"

class UIReAlignManager : public SystemLogic,
    public ETNode<ETUIReAlignManager> {
public:

    UIReAlignManager();
    virtual ~UIReAlignManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETUIReAlignManager
    void ET_setLayoutDirty(EntityId entId) override;
    void ET_doReAlign() override;

private:

    std::unordered_set<EntityId> dirtyLayouts;
    bool resizing;
};

#endif /* __UI_REALIGN_MANAGER_HPP__ */