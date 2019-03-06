#ifndef __STRING_FORAMT_HPP__
#define __STRING_FORAMT_HPP__

#include "Core/Buffer.hpp"

namespace Core {

template<typename T>
T ConvertToPrintable(T& val) {
    return std::forward<T>(val);
}

const char* ConvertToPrintable(const std::string& str);


template <typename... ArgsT>
void StringFormatImpl(Buffer& buff, const std::string& format, const ArgsT& ... args) {
    auto resStrSize = std::snprintf(static_cast<char*>(buff.getData()), buff.getSize(), format.c_str(), args...);
    if(resStrSize + 1 > buff.getSize()) {
        buff.resize(resStrSize + 1);
        StringFormatImpl(buff, format, args...);
    }
}

} // namespace Core

template<typename ... ArgsT>
std::string StringFormat(const std::string& format, const ArgsT& ... args)
{
    Buffer buff(format.size());
    Core::StringFormatImpl(buff, format.c_str(), Core::ConvertToPrintable(args)...);
    return buff.getString();
}

#endif /* __STRING_FORAMT_HPP__ */