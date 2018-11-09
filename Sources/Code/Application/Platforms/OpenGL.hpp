#ifdef DESKTOP_APP_BUILD

  #ifdef _WIN32
    #ifndef APIENTRY
      #define APIENTRY __stdcall
    #endif
  #endif

  #ifdef _WINDOWS_
    #ifndef GLAD_DEBUG
      #error GLAD ERROR: windows.h was included
    #endif
  #endif

  #include <glad/glad.h>
  #include <GLFW/glfw3.h>

#else

    #error OpenGL API not specified

#endif /* DESKTOP_APP_BUILD */