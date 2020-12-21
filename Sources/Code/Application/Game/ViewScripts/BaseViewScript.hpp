#ifndef __BASE_VIEW_SCRIPT_HPP__
#define __BASE_VIEW_SCRIPT_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIViewScript.hpp"

class BaseViewScript : public EntityLogic,
    public ETNode<ETUIViewScript> {
public:

    BaseViewScript();
    virtual ~BaseViewScript();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETUIViewScript
    void ET_onViewOpened() override;
    void ET_onViewClosed() override;
    void ET_onViewGetFocus() override;
    void ET_onViewLostFocus() override;
    void ET_onEvent(const UIEvent& event) override;

protected:

    virtual void onEvent(const UIEvent& event) = 0;

private:

    bool hasFocus;
};

#endif /* __BASE_VIEW_SCRIPT_HPP__ */