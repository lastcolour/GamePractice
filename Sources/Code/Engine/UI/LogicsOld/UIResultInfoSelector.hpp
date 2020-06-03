#ifndef __UI_RESULT_INFO_SELECTOR_HPP__
#define __UI_RESULT_INFO_SELECTOR_HPP__

#include "Entity/EntityLogic.hpp"

#include <string>

class UIResultInfoSelector : public EntityLogic {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIResultInfoSelector();
    virtual ~UIResultInfoSelector();

    // EntityLogic
    bool init() override;
    void deinit() override;

private:

    std::string normalExtend;
    std::string newHighScoreExtend;
};

#endif /* __UI_RESULT_INFO_SELECTOR_HPP__ */