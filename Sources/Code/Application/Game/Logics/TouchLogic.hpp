#ifndef __TOUCH_LOGIC_HPP__
#define __TOUCH_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "Game/GameETInterfaces.hpp"

class TouchLogic : public GameLogic<TouchLogic>,
    public ETNode<ETTouchLogic> {
public:

    TouchLogic();
    virtual ~TouchLogic();

    bool init(const JSONNode& node) override;
    void update() override;

    // ET_TouchLogic
    void ET_onTouch(int x, int y) override;
};

#endif /* __TOUCH_LOGIC_HPP__ */