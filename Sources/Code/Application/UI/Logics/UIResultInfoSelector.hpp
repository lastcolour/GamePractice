#ifndef __UI_RESULT_INFO_SELECTOR_HPP__
#define __UI_RESULT_INFO_SELECTOR_HPP__

#include "Entity/EntityLogic.hpp"

#include <string>

class UIResultInfoSelector : public EntityLogic {
public:

    UIResultInfoSelector();
    virtual ~UIResultInfoSelector();

    // EntityLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

private:

    std::string normalExtend;
    std::string newHighScoreExtend;
};

#endif /* __UI_RESULT_INFO_SELECTOR_HPP__ */