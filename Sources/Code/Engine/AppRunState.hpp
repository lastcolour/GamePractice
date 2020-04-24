#ifndef __APP_RUN_STATE_HPP__
#define __APP_RUN_STATE_HPP__

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"

class AppRunState : public SystemLogic,
    public ETNode<ETAppRunStateEvents> {
public:

    AppRunState();
    virtual ~AppRunState() = default;

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETAppRunStateEvents
    void ET_onTerminate() override;
    void ET_onPause() override;
    void ET_onResume() override;
    bool ET_isNeedRun() const override;

public:

    bool needRun;
};

#endif /* __APP_RUN_STATE_HPP__  */