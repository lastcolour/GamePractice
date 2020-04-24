#ifndef __UI_LIST_HPP__
#define __UI_LIST_HPP__

#include "UI/Logics/UIBaseBox.hpp"

struct OffsetData;

class UIList : public UIBaseBox,
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
    void initOffset(OffsetData& offsetData) const;
    Vec2i caclCenterUpdateOffset(OffsetData& offsetData, const AABB2Di& elemBox, const Margin& elemMargin) const;

private:

    UIListType listType;
    std::vector<EntityId> children;
};

#endif /* __UI_LIST_HPP__ */