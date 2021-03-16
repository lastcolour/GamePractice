#ifndef __ET_APLLICATION_HPP__
#define __ET_APLLICATION_HPP__

struct ETAppRunStateEvents {
    virtual ~ETAppRunStateEvents() = default;
    virtual void ET_onTerminate() = 0;
    virtual void ET_onPause() = 0;
    virtual void ET_onResume() = 0;
};

#endif /* __ET_APLLICATION_HPP__ */