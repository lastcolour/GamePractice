#include "Platforms/Android/AndroidSurface.hpp"
#include "Platforms/Android/AndroidPlatformHandler.hpp"

#include <EGL/eglext.h>
#include <android/native_activity.h>

namespace {

const char* getEGLErrorStr() {
    EGLint errCode = eglGetError();
    switch(errCode) {
        case EGL_BAD_MATCH: {
            return "EGL_BAD_MATCH";
        }
        case EGL_BAD_DISPLAY: {
            return "EGL_BAD_DISPLAY";
        }
        case EGL_NOT_INITIALIZED: {
            return "EGL_NOT_INITIALIZED";
        }
        case EGL_BAD_CONFIG: {
            return "EGL_BAD_CONFIG";
        }
        case EGL_BAD_CONTEXT: {
            return "EGL_BAD_CONTEXT";
        }
        case EGL_BAD_ATTRIBUTE: {
            return "EGL_BAD_ATTRIBUTE";
        }
        case EGL_BAD_ALLOC: {
            return "EGL_BAD_ALLOC";
        }
        case EGL_BAD_SURFACE: {
            return "EGL_BAD_SURFACE";
        }
        case EGL_BAD_ACCESS: {
            return "EGL_BAD_ACCESS";
        }
        case EGL_BAD_NATIVE_PIXMAP: {
            return "EGL_BAD_NATIVE_PIXMAP";
        }
        case EGL_BAD_NATIVE_WINDOW: {
            return "EGL_BAD_NATIVE_WINDOW";
        }
        case EGL_BAD_CURRENT_SURFACE: {
            return "EGL_BAD_CURRENT_SURFACE";
        }
        case EGL_CONTEXT_LOST: {
            return "EGL_CONTEXT_LOST";
        }
        default: {
           return "NONE";
        }
    }
}

const EGLint EGLContexAttribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 3,
    EGL_NONE
};

EGLConfig chooseEGLConfig(EGLDisplay eglDisplay) {
    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[1]);
    int numConfigs = 1;
    {
        const EGLint es3_rgba_24[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_NONE
        };

        if(eglChooseConfig(eglDisplay, es3_rgba_24, supportedConfigs.get(), 1, &numConfigs) == EGL_TRUE) {
            LogInfo("[chooseEGLConfig] EGLConfig: es3_rgba_24");
            return supportedConfigs[0];
        }
    }

    {
        const EGLint es3_rgba_16[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_NONE
        };

        if(eglChooseConfig(eglDisplay, es3_rgba_16, supportedConfigs.get(), 1, &numConfigs) == EGL_TRUE) {
            LogInfo("[chooseEGLConfig] Config: es3_rgba_16");
            return supportedConfigs[0];
        }
    }

    return nullptr;
}

} // namespace

AndroidSurface::AndroidSurface() :
    display(EGL_NO_DISPLAY),
    context(EGL_NO_CONTEXT),
    surface(EGL_NO_SURFACE),
    nativeWindow(nullptr),
    config(nullptr) {
}

AndroidSurface::~AndroidSurface() {
}

bool AndroidSurface::init() {
    ETNode<ETTimerEvents>::connect(getEntityId());
    ETNode<ETAndroidActivityEvents>::connect(getEntityId());
    ETNode<ETSurface>::connect(getEntityId());
    return true;
}

void AndroidSurface::deinit() {
    ETNode<ETTimerEvents>::disconnect();
    ETNode<ETAndroidActivityEvents>::disconnect();
    ETNode<ETSurface>::disconnect();
}

bool AndroidSurface::createEGLDisplay() {
    auto defaultDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (defaultDisplay == EGL_NO_DISPLAY) {
        LogError("[AndroidSurface::createEGLDisplay] Can't get default EGL display. Error: %s", getEGLErrorStr());
        return false;
    }

    if (eglInitialize(defaultDisplay, 0, 0) != EGL_TRUE) {
        LogError("[AndroidSurface::createEGLDisplay] Can't initialize default display. Error: %s", getEGLErrorStr());
        return false;
    }

    if(!(config = chooseEGLConfig(defaultDisplay))) {
        LogError("[AndroidSurface::createEGLDisplay] Can't choose EGL config for display. Error: %s", getEGLErrorStr());
        eglTerminate(defaultDisplay);
        return false;
    }

    EGLint nativeVisualId;
    if(!eglGetConfigAttrib(defaultDisplay, config, EGL_NATIVE_VISUAL_ID, &nativeVisualId)) {
        LogError("[AndroidSurface::createEGLDisplay] Can't queury EGL_NATIVE_VISUAL_ID from default display. Error: %s", getEGLErrorStr());
        eglTerminate(defaultDisplay);
        return false;
    }

    ANativeWindow_setBuffersGeometry(nativeWindow, 0, 0, nativeVisualId);

    auto windowSurface = eglCreateWindowSurface(defaultDisplay, config, nativeWindow, nullptr);
    if(windowSurface == EGL_NO_SURFACE) {
        LogError("[AndroidSurface::createEGLDisplay] Can't create window surface. Error: %s", getEGLErrorStr());
        eglTerminate(defaultDisplay);
        return false;
    }
    display = defaultDisplay;
    surface = windowSurface;
    return true;
}

bool AndroidSurface::createEGLContext() {
    auto newContext = eglCreateContext(display, config, EGL_NO_CONTEXT, EGLContexAttribs);
    if(newContext == EGL_NO_CONTEXT) {
        LogError("[AndroidSurface::createEGLContext] Can't create context. Error: %s", getEGLErrorStr());
        return false;
    }
    if(eglMakeCurrent(display, surface, surface, context) != EGL_TRUE) {
        LogError("[AndroidSurface::createEGLContext] Can't make context current. Error: %s", getEGLErrorStr());
        return false;
    }
    context = newContext;
    return true;
}

void AndroidSurface::suspendEGLContext() {
    if(surface != EGL_NO_SURFACE) {
        eglDestroySurface(display, surface);
        surface = EGL_NO_SURFACE;
    }
}

void AndroidSurface::updateEGLContext() {
    if(display == EGL_NO_DISPLAY) {
        if(!createEGLDisplay()) {
            return;
        }
    }
    // restore context
    if(context != EGL_NO_CONTEXT) {
        if(eglMakeCurrent(display, surface, surface, context) == EGL_TRUE) {
            LogInfo("[AndroidSurface::updateEGLContext] Previous context was restored succsefully");
            return;
        }
    }
    createEGLContext();
}

void AndroidSurface::destroyEGLContext() {
    if (display != EGL_NO_DISPLAY) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
        }
        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
        }
        eglTerminate(display);
    }
    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;
    nativeWindow = nullptr;
    config = nullptr;
}

void AndroidSurface::ET_onActivityEvent(ActivityEventType eventType) {
    switch (eventType)
    {
    case ActivityEventType::OnNativeWindowChanged:
        {
            ANativeWindow* newNativeWindow = GetAndroindPlatformHandler()->getWindow();
            if(nativeWindow) {
                if(!newNativeWindow) {
                    suspendEGLContext();
                } else if(newNativeWindow != nativeWindow) {
                    destroyEGLContext();
                }
                nativeWindow = nullptr;
            }
            if(newNativeWindow) {
                nativeWindow = newNativeWindow;
                updateEGLContext();
            }
            break;
        }
    case ActivityEventType::OnWindowFocusGet:
        break;
    case ActivityEventType::OnWindowFocusLost:
        break;
    default:
        break;
    }
}

void AndroidSurface::ET_onTick(float dt) {
    if(!ET_isValid()) {
        return;
    }
    Vec2i currSize(0);
    if(EGL_TRUE != eglQuerySurface(display, surface, EGL_HEIGHT, &currSize.y) ||
        EGL_TRUE != eglQuerySurface(display, surface, EGL_WIDTH, &currSize.x)) {
        LogWarning("[AndroidSurface::ET_onTick] Can't query surface size. Error: %s", getEGLErrorStr());
        return;
    }
    if(currSize != size) {
        size = currSize;
        ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceResize, size);
    }
}

bool AndroidSurface::ET_show() {
    assert(false && "Can't show android surface");
    LogError("[AndroidSurface::ET_show] Can't show android surface");
    return false;
}

bool AndroidSurface::ET_hide() {
    assert(false && "Can't hide android surface");
    LogError("[AndroidSurface::ET_hide] Can't hide android surface");
    return false;
}

void AndroidSurface::ET_close() {
    assert(false && "Can't close android surface");
    LogError("[AndroidSurface::ET_close] Can't close androind surface");
}

Vec2i AndroidSurface::ET_getSize() const {
    if(ET_isValid()) {
        return size;
    }
    return Vec2i(0);
}

void AndroidSurface::ET_swapBuffers() {
    if(!ET_isValid()) {
        assert(ET_isValid() && "Can't swap buffer in invalid surface");
        LogError("[AndroidSurface::ET_swapBuffers] Can't swap buffer in invalid surface");
        return;
    }
    eglSwapBuffers(display, surface);
}

GLContextType AndroidSurface::ET_getGLContextType() const {
    if(ET_isValid()) {
        return GLContextType::ES30;
    }
    return GLContextType::None;
}

bool AndroidSurface::ET_isVisible() const {
    return ET_isValid();
}

bool AndroidSurface::ET_isValid() const {
    return context != EGL_NO_CONTEXT
        && surface != EGL_NO_SURFACE
        && display != EGL_NO_DISPLAY;
}