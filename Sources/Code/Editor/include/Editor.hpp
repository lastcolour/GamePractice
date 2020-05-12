#ifndef __EDITOR_HPP__
#define __EDITOR_HPP__

#ifdef _WIN32
  #define LIB_EXPORT __declspec(dllexport)
#else
  #define LIB_EXPORT
#endif

extern "C" {

LIB_EXPORT int Initiliaze();

LIB_EXPORT const char* GetReflectModel();

LIB_EXPORT void DeInitialize();

} // extern "C"

#endif /* __EDITOR_HPP__ */