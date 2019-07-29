#include "Core/StringFormat.hpp"

namespace Core {

char* StringCopyUnsafe(char *dest, const char *src) {
   char* ptr = dest;
   while((*dest++ = *src++));
   return ptr;
}

const char* ConvertToPrintable(const std::string& str) {
    return str.c_str();
}

} // namespace Core