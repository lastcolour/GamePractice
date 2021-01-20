#include "Android/AndroidSurface.hpp"
#include "Android/AndroidPlatformHandler.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>

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
    EGLConfig config;
    int numConfigs = 0;

    const EGLint attribList[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_NONE
    };

    if(eglChooseConfig(eglDisplay, attribList, &config, 1, &numConfigs) == EGL_TRUE) {
        return config;
    } else {
        LogError("[chooseEGLConfig] Can't choose EGL display attribs");
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
    ETNode<ETInputUpdateTask>::connect(getEntityId());
    ETNode<ETAndroidActivityEvents>::connect(getEntityId());
    ETNode<ETSurface>::connect(getEntityId());
    return true;
}

void AndroidSurface::deinit() {
    ETNode<ETInputUpdateTask>::disconnect();
    ETNode<ETAndroidActivityEvents>::disconnect();
    ETNode<ETSurface>::disconnect();
}

bool AndroidSurface::createEGLSurface() {
    surface = eglCreateWindowSurface(display, config, nativeWindow, nullptr);
    if(surface == EGL_NO_SURFACE) {
        LogError("[AndroidSurface::createEGLSurface] Can't create window surface. Error: %s", getEGLErrorStr());
        return false;
    }
    Vec2i currSize(0);
    if(EGL_TRUE != eglQuerySurface(display, surface, EGL_HEIGHT, &currSize.y) ||
        EGL_TRUE != eglQuerySurface(display, surface, EGL_WIDTH, &currSize.x)) {
        LogError("[AndroidSurface::createEGLSurface] Can't query surface size. Error: %s", getEGLErrorStr());
        return false;
    }
    size = currSize;
    return true;
}

bool AndroidSurface::createEGLDisplay() {
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(display == EGL_NO_DISPLAY) {
        LogError("[AndroidSurface::createEGLDisplay] Can't get default EGL display. Error: %s", getEGLErrorStr());
        return false;
    }

    if(eglInitialize(display, 0, 0) != EGL_TRUE) {
        LogError("[AndroidSurface::createEGLDisplay] Can't initialize default display. Error: %s", getEGLErrorStr());
        return false;
    }

    if(!(config = chooseEGLConfig(display))) {
        LogError("[AndroidSurface::createEGLDisplay] Can't choose EGL config for display. Error: %s", getEGLErrorStr());
        return false;
    }

    EGLint nativeVisualId;
    if(!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &nativeVisualId)) {
        LogError("[AndroidSurface::createEGLDisplay] Can't queury EGL_NATIVE_VISUAL_ID from default display. Error: %s", getEGLErrorStr());
        return false;
    }

    ANativeWindow_setBuffersGeometry(nativeWindow, 0, 0, nativeVisualId);

    if(!createEGLSurface()) {
        return false;
    }

    return true;
}

bool AndroidSurface::createEGLContext() {
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, EGLContexAttribs);
    if(context == EGL_NO_CONTEXT) {
        LogError("[AndroidSurface::createEGLContext] Can't create context. Error: %s", getEGLErrorStr());
        return false;
    }
    if(eglMakeCurrent(display, surface, surface, context) != EGL_TRUE) {
        LogError("[AndroidSurface::createEGLContext] Can't make context current. Error: %s", getEGLErrorStr());
        return false;
    }
    LogInfo("[AndroidSurface::createEGLContext] EGL context created");
    return true;
}

void AndroidSurface::onNativeWindowCreated() {
    if(display != EGL_NO_DISPLAY && context != EGL_NO_CONTEXT) {
        if(createEGLSurface()) {
            if(eglMakeCurrent(display, surface, surface, context) == EGL_TRUE) {
                LogInfo("[AndroidSurface::onNativeWindowCreated] Previous context was restored succssefully");
                ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceShown);
                return;
            }
            LogWarning("[AndroidSurface::onNativeWindowCreated] Can't restore previous context: %s", getEGLErrorStr());
            if(createEGLContext()) {
                LogInfo("[AndroidSurface::onNativeWindowCreated] Create new context using previous display & new surface");
                ET_SendEvent(&ETRenderContextEvents::ET_onContextDestroyed);
                ET_SendEvent(&ETRenderContextEvents::ET_onContextCreated);
                ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceShown);
                return;
            }
            LogWarning("[AndroidSurface::onNativeWindowCreated] Can't create new context using previous surface & display");
        }
        display = EGL_NO_DISPLAY;
        surface = EGL_NO_SURFACE;
        context = EGL_NO_CONTEXT;
    }

    if(createEGLDisplay()) {
        if(createEGLContext()) {
            LogInfo("[AndroidSurface::onNativeWindowCreated] Create new context");
            ET_SendEvent(&ETRenderContextEvents::ET_onContextCreated);
            ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceShown);
            return;
        }
    }
    LogFatal("[AndroidSurface::onNativeWindowCreated] Can't create EGL context");
}

void AndroidSurface::onNativeWindowDestoryed() {
    LogInfo("[AndroidSurface::onNativeWindowDestoryed] Suspend EGL Context");
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if(surface != EGL_NO_SURFACE) {
        eglDestroySurface(display, surface);
        surface = EGL_NO_SURFACE;
    }
}

void AndroidSurface::ET_onActivityEvent(ActivityEventType eventType) {
    switch(eventType)
    {
    case ActivityEventType::OnNativeWindowChanged:
        {
            ANativeWindow* newNativeWindow = GetAndroindPlatformHandler()->getWindow();
            if(nativeWindow) {
                onNativeWindowDestoryed();
                nativeWindow = nullptr;
                ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceDestroyed);
            }
            nativeWindow = newNativeWindow;
            if(nativeWindow) {
                onNativeWindowCreated();
                ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceCreated);
                ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceResized, size);
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

void AndroidSurface::ET_updateInput() {
    if(!ET_isValid()) {
        return;
    }
    Vec2i currSize(0);
    if(EGL_TRUE != eglQuerySurface(display, surface, EGL_HEIGHT, &currSize.y) ||
        EGL_TRUE != eglQuerySurface(display, surface, EGL_WIDTH, &currSize.x)) {
        LogWarning("[AndroidSurface::ET_onSystemTick] Can't query surface size. Error: %s", getEGLErrorStr());
        return;
    }
    if(currSize != size) {
        size = currSize;
        ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceResized, size);
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