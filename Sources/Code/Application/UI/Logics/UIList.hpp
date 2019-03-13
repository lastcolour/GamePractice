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
    Vec2i ET_getElemOffset(int idx) const override;
    ListType ET_getListType() const override;

protected:

    void addElement(EntityId entId);

private:

    void calcResListBox();

private:

    std::vector<EntityId> children;
};

#endif /* __UI_LIST_HPP__ */