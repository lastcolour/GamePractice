#ifndef __DOUBLE_TAP_DETECTION_HPP__
#define __DOUBLE_TAP_DETECTION_HPP__

#include "Platform/ETSurface.hpp"

class DoubleTapDetection {
public:

    struct TapInfo {
        EntityId targetId;
        int tapCount;
    };

public:

    DoubleTapDetection();
    ~DoubleTapDetection();

    void onTouchEvent(EntityId targetId, const TouchEvent& event);
    TapInfo getTapInfo() const;
    void reset();

private:

    struct TapEvent {
        TouchEvent touch;
        EntityId targetId;
    };

private:

    TapEvent prevTap;
    int tapCount;
};

#endif /* __DOUBLE_TAP_DETECTION_HPP__ */