#ifndef __ET_EDITOR_INTERFACES_HPP__
#define __ET_EDITOR_INTERFACES_HPP__

struct ETEntityEditorHelper {
    virtual ~ETEntityEditorHelper() = default;
    virtual void ET_setFocusEntity(EntityId newFocusEntId) = 0;
};

#endif /* __ET_EDITOR_INTERFACES_HPP__ */