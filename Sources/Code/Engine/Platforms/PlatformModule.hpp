#ifndef __PLATFORM_MODULE_HPP__
#define __PLATFORM_MODULE_HPP__

#ifdef PLATFORM_ANDROID
  #include "Platforms/Android/AndroidModule.hpp"
  using PlatformModule = AndroidModule;
#else
  #include "Platforms/Desktop/DesktopModule.hpp"
  using PlatformModule = DesktopModule;
#endif

#endif /* __PLATFORM_MODULE_HPP__ */