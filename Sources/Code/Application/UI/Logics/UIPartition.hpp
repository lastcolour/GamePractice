#ifndef __UI_PARTITION_HPP__
#define __UI_PARTITION_HPP__

#include "UI/Logics/UIBaseBox.hpp"

class UIPartition : public UIBaseBox {
public:

    UIPartition();
    virtual ~UIPartition();

    // namespace
    bool serialize(const JSONNode& node) override;
    bool init() override;

private:

    EntityId serializeNode(const JSONNode& node);
    EntityId serializeAsList(UIListType listType, const JSONNode& node);
    EntityId serializeAsBox(const JSONNode& node);
    EntityId serializeAsObject(const JSONNode& node);
    EntityId serializeAsSimplified(const JSONNode& node);
    void serializeChildren(EntityId rootEntId, const JSONNode& node);
};

#endif /* __UI_PARTITION_HPP__ */