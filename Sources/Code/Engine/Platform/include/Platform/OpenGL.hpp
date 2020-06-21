#if defined PLATFORM_WINDOWS || defined PLATFORM_LINUX

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

#elif defined PLATFORM_ANDROID

  #include <GLES3/gl3.h>
  #include <GLES3/gl3ext.h>

#else

    #error OpenGL API not specified

#endif /* DESKTOP_APP_BUILD */