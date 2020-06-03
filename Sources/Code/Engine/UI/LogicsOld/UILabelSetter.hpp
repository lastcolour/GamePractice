#ifndef __UI_LABEL_SETTER_HPP__
#define __UI_LABEL_SETTER_HPP__

#include "Entity/EntityLogic.hpp"

class UILabelSetter : public EntityLogic {
public:

    static void Reflect(ReflectContext& ctx);

public:

    enum class EValueType {
        None = 0,
        GameScore,
        GameHighScore
    };

public:

    UILabelSetter();
    virtual ~UILabelSetter();

    // EntityLogic
    bool init() override;
    void deinit() override;

private:

    EValueType valueType;
};

#endif /* __UI_LABEL_SETTER_HPP__ */