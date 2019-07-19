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
    void ET_addChildElement(EntityId newElemId) override;

protected:

    UIListType listType;

private:

    void calcList();
    void calcResListBox();
    int sfiftCenterByOffset(int offset, Vec2i& center) const;

private:

    std::vector<EntityId> children;
};

#endif /* __UI_LIST_HPP__ */