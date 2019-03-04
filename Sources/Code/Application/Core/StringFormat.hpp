#ifndef __STRING_FORAMT_HPP__
#define __STRING_FORAMT_HPP__

#include "Core/Buffer.hpp"

template<typename T>
T ConvertToPrintable(const T& val) {
    return val;
}

const char* ConvertToPrintable(const std::string& str);

template <typename... Args>
void StringFormatImpl(Buffer& buff, const std::string& format, Args&& ... args) {
    auto resStrSize = std::snprintf(static_cast<char*>(buff.getData()), buff.getSize(), format.c_str(), args...);
    if(resStrSize + 1 > buff.getSize()) {
        buff.resize(resStrSize + 1);
        StringFormatImpl(buff, format, std::forward<Args>(args)...);
    }
}

template<typename ... ArgsT>
std::string StringFormat(const std::string& format, const ArgsT& ... args)
{
    Buffer buff(format.size());
    StringFormatImpl(buff, format.c_str(), ConvertToPrintable(args)...);
    return buff.getString();
}

#endif /* __STRING_FORAMT_HPP__ */