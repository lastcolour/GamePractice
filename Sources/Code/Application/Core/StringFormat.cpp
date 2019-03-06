#include "Core/StringFormat.hpp"

namespace Core {

const char* ConvertToPrintable(const std::string& str) {
    return str.c_str();
}

} // namespace Core