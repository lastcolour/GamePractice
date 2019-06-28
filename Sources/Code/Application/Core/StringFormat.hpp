#ifndef __STRING_FORAMT_HPP__
#define __STRING_FORAMT_HPP__

#include "Core/Buffer.hpp"

#include <cstring>

namespace Core {

template<typename T>
T ConvertToPrintable(T& val) {
    return std::forward<T>(val);
}

const char* ConvertToPrintable(const std::string& str);

template <typename... ArgsT>
void StringFormatImpl(Buffer& buff, const std::string& format, const ArgsT& ... args) {
    int resStrSize = std::snprintf(static_cast<char*>(buff.getData()), buff.getSize(), format.c_str(), args...);
    if(resStrSize < 0) {
        return;
    }
    size_t resSize = static_cast<size_t>(resStrSize + 1);
    if(resSize > buff.getSize()) {
        buff.resize(resSize);
        StringFormatImpl(buff, format, args...);
    }
}

} // namespace Core

template<typename ... ArgsT>
std::string StringFormat(const std::string& format) {
    return format;
}

template<typename ... ArgsT>
std::string StringFormat(const std::string& format, const ArgsT& ... args) {
    Buffer buff(format.size() + 1);
    if(sizeof...(args) > 0u) {
        Core::StringFormatImpl(buff, format.c_str(), Core::ConvertToPrintable(args)...);
    } else {
        size_t i = 0u;
        auto destPtr = static_cast<char*>(buff.getData());
        while((destPtr[i] = format[i]) != '\0') {
            ++i;
        }
    }
    return buff.getString();
}

#endif /* __STRING_FORAMT_HPP__ */