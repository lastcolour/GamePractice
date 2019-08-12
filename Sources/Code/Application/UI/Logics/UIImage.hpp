#ifndef __UI_IMAGER_HPP_
#define __UI_IMAGER_HPP_

#include "UI/Logics/UIBaseBox.hpp"

class UIImage : public UIBaseBox {
public:

    UIImage();
    virtual ~UIImage();

    // UIBaseBox
    bool serialize(const JSONNode& node) override;
    bool init() override;

private:

    EntityId imageRendererId;
};

#endif /* __UI_IMAGER_HPP_ */