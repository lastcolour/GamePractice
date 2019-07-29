#ifndef __UI_LIST_HPP__
#define __UI_LIST_HPP__

#include "UI/Logics/UIBox.hpp"

class UIList : public UIBox,
    public ETNode<ETUIList> {
public:

    UIList();
    virtual ~UIList();

    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETUIList
    void ET_setType(UIListType newListType) override;

    // ETUIBox
    void ET_boxResize() override;
    void ET_onChildAdded(EntityId newElemId) override;

private:

    void calcList();
    AABB2Di getAligntBox(const AABB2Di& elemBox) const;
    Vec2i caclCenterUpdateOffset(Vec2i& offset, const AABB2Di& elemBox);

private:

    UIListType listType;
    std::vector<EntityId> children;
};

#endif /* __UI_LIST_HPP__ */