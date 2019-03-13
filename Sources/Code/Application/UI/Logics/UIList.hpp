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

protected:

    void addElement(EntityId entId);

private:

    std::vector<EntityId> children;
};

#endif /* __UI_LIST_HPP__ */