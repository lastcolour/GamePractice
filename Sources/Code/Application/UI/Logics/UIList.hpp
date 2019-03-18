#ifndef __UI_LIST_HPP__
#define __UI_LIST_HPP__

#include "UI/Logics/UIBox.hpp"

enum class ListType {
    Vertical = 0,
    Horizontal
};

class UIList : public UIBox,
    public ETNode<ETUIList> {
public:

    UIList();
    virtual ~UIList();

    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETUIList
    void ET_addElement(EntityId newElemId) override;

    // ETSurfaceEvents
    void ET_onSurfaceResize(const Vec2i& size) override;

protected:

    ListType listType;

private:

    void calcList();
    void calcResListBox();

private:

    std::vector<EntityId> children;
};

#endif /* __UI_LIST_HPP__ */