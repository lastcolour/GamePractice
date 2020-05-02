#ifndef __UI_LABEL_SETTER_HPP__
#define __UI_LABEL_SETTER_HPP__

#include "Entity/EntityLogic.hpp"

class UILabelSetter : public EntityLogic {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UILabelSetter();
    virtual ~UILabelSetter();

    // EntityLogic
    bool init() override;
    void deinit() override;

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