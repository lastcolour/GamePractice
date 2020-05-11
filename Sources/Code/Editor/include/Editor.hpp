#ifndef __EDITOR_HPP__
#define __EDITOR_HPP__

#ifdef _WIN32
  #define LIB_EXPORT __declspec(dllexport)
#else
  #define LIB_EXPORT
#endif

extern "C" {

int LIB_EXPORT Initiliaze();

const char* LIB_EXPORT GetReflectModel();

void LIB_EXPORT DeInitialize();

} // extern "C"

#endif /* __EDITOR_HPP__ */