#ifndef __ET_DRAW_COMMANDS_HPP__
#define __ET_DRAW_COMMANDS_HPP__

#include "Commands/Commands.hpp"

class BaseDrawCommandExectuor;
enum class EDrawContentFilter;

struct ETDrawCommandsManager {
    virtual ~ETDrawCommandsManager() = default;
    virtual DrawCmd* ET_createDrawCmd(EDrawCmdType cmdType) = 0;
    virtual void ET_preRender() = 0;
    virtual void ET_scheduleDrawCmdEvent(std::function<void(BaseDrawCommandExectuor*)>&& func, EDrawCmdType cmdType) = 0;
    virtual void ET_renderToDefaultFBO() = 0;
    virtual void ET_renderToBuffer(void* out, EDrawContentFilter filter) = 0;
};

#endif /* __ET_DRAW_COMMANDS_HPP__ */