#ifndef __UI_LABEL_SETTER_HPP__
#define __UI_LABEL_SETTER_HPP__

#include "Entity/EntityLogic.hpp"

class UILabelSetter : public EntityLogic {
public:

    UILabelSetter();
    virtual ~UILabelSetter();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

private:

    enum class EValueType {
        None = 0,
        GameScore,
        GameHighScore
    };

private:

    EValueType valueType;
};

#endif /* __UI_LABEL_SETTER_HPP__ */