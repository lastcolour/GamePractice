#ifndef __DESKTIO_LOG_STREAM_HPP__
#define __DESKTIO_LOG_STREAM_HPP__

#include "Platforms/LogStream.hpp"

class DesktopLogStream : public LogStream {
public:

    void logMessage(LogLevel logLvl, const std::string& msg) override;
};

#endif /* __DESKTIO_LOG_STREAM_HPP__ */