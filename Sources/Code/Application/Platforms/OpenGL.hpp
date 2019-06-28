#if defined APP_BUILD_PLATFORM_WINDOWS || defined APP_BUILD_PLATFORM_LINUX

  #ifdef _WIN32
    #ifndef APIENTRY
      #define APIENTRY __stdcall
    #endif
  #endif

  #include <glad/glad.h>

  #ifdef _WINDOWS_
    #ifndef GLAD_DEBUG
      #error GLAD ERROR: windows.h was included
    #endif
  #endif

#else

    #error OpenGL API not specified

#endif /* DESKTOP_APP_BUILD */