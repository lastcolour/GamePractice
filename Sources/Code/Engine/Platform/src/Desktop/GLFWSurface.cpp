#include "Desktop/GLFWSurface.hpp"
#include "Core/ETApplication.hpp"
#include "Platform/OpenGL.hpp"

#include <GLFW/glfw3.h>

#include <cassert>

namespace {

const int DEF_WIDTH = 600;
const int DEF_HEIGHT = 1024;
const char* DEF_WINDOW_NAME = "Game01";

} // namespace

class GlfwLibInitData {
public:

    GlfwLibInitData() :
        activeSurface(nullptr),
        status(false) {}

    ~GlfwLibInitData() {
        if(status) {
            glfwTerminate();
        }
        status = false;
    }

    bool init() {
        if(!status) {
            status = glfwInit();
        }
        return status;
    }

    bool setActiveSurface(GLFWSurface* surface) {
        if(!activeSurface) {
            activeSurface = surface;
            return true;
        } else {
            assert(false && "Try Create second surface");
            LogError("[GlfwLibInitData:setActiveSurface] Can't set other surface as active");
            return false;
        }
    }

    void resetActiveSurface(GLFWSurface* surface) {
        if(surface == activeSurface) {
            activeSurface = nullptr;
        } else {
            assert(false && "Try reset other surface");
            LogError("[GlfwLibInitData:resetActiveSurface] Can't reset other surface");
        }
    }

    GLFWSurface* getActiveSurface(GLFWwindow* window) {
        if(!activeSurface) {
            LogError("[GlfwLibInitData::getActiveSurface] No active surface!");
            return nullptr;
        }
        if(activeSurface->getWindow() != window) {
            LogError("[GlfwLibInitData::getActiveSurface] Invalid window from active surface");
            return nullptr;
        }
        return activeSurface;
    }

private:

    GLFWSurface* activeSurface;
    bool status;
};

std::unique_ptr<GlfwLibInitData> GLFWSurface::GLFW = nullptr;

GLFWSurface::GLFWSurface() :
    window(nullptr),
    size(0),
    editorMode(false) {
}

GLFWSurface::~GLFWSurface() {
    if(window) {
        glfwDestroyWindow(window);
    }
    GLFW->resetActiveSurface(this);
    window = nullptr;
}

const GLFWwindow* GLFWSurface::getWindow() const {
    return window;
}

bool GLFWSurface::init() {
    if(!GLFW) {
        GLFW.reset(new GlfwLibInitData);
    }
    if(!GLFW->init()) {
        LogError("[GLFWSurface::init] Can't init GLFW library");
        return false;
    }
    if(!GLFW->setActiveSurface(this)) {
        return false;
    }

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    size = Vec2i(DEF_WIDTH, DEF_HEIGHT);
    window = glfwCreateWindow(size.x, size.y, DEF_WINDOW_NAME, nullptr, nullptr);
    if(!window) {
        const char* errStr = nullptr;
        int errCode = glfwGetError(&errStr);
        LogError("[GLFWSurface] Can't create window: '%s' (%d)", errStr, errCode);
        return false;
    }

    glfwSetFramebufferSizeCallback(window, SetFramebufferSizeCallback);
    glfwSetMouseButtonCallback(window, SetMouseButtonCallback);
    glfwSetCursorPosCallback(window, SetCursorePosCallback);
    glfwSetKeyCallback(window, SetKeyboardButtonCallback);
    glfwSetWindowIconifyCallback(window, SetIconfyCallback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

    // Create surface hidden
    // ET_show();

    ETNode<ETSurface>::connect(getEntityId());
    ETNode<ETInputUpdateTask>::connect(getEntityId());

    return true;
}

void GLFWSurface::deinit() {
    ETNode<ETSurface>::disconnect();
}

void GLFWSurface::ET_updateInput() {
    glfwPollEvents();
    if(window != nullptr && glfwWindowShouldClose(window)) {
        ET_SendEvent(&ETAppRunStateEvents::ET_onTerminate);
    }
}

bool GLFWSurface::ET_isVisible() const {
    if(window) {
        return glfwGetWindowAttrib(window, GLFW_VISIBLE);
    }
    return false;
}

bool GLFWSurface::ET_show() {
    assert(!editorMode && "Can't show surface in editor mode");
    if(window) {
        glfwShowWindow(window);
        glfwFocusWindow(window);
        ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceShown);
        return true;
    }
    return false;
}

bool GLFWSurface::ET_hide() {
    assert(!editorMode && "Can't hide surface in editor mode");
    if(window && ET_isVisible()) {
        glfwHideWindow(window);
        ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceHidden);
        return true;
    }
    return false;
}

void GLFWSurface::ET_close() {
    assert(!editorMode && "Can't close surface in editor mode");
    if(window) {
        glfwSetWindowShouldClose(window, true);
    }
}

void GLFWSurface::ET_swapBuffers() {
    assert(!editorMode && "Can't swaps buffers in editor mode");
    if(window) {
        glfwSwapBuffers(window);
    }
}

Vec2i GLFWSurface::ET_getSize() const {
    return size;
}

bool GLFWSurface::ET_canRender() const {
    if(window) {
        return !glfwWindowShouldClose(window);
    }
    return false;
}

void GLFWSurface::SetCursorePosCallback(GLFWwindow* window, double x, double y) {
    auto surface = GLFW->getActiveSurface(window);
    if(!surface) {
        return;
    }
    Vec2i pt(static_cast<int>(x), static_cast<int>(y));
    pt.y = surface->size.y - pt.y;

    TouchEvent event;
    event.actionType = EActionType::Move;
    event.pt = pt;
    event.eventT = TimePoint::GetNowTime();
    ET_QueueEvent(&ETInputEvents::ET_onTouch, event);
}

void GLFWSurface::SetMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    (void)mods;
    auto surface = GLFW->getActiveSurface(window);
    if(!surface) {
        return;
    }
    if(button != GLFW_MOUSE_BUTTON_LEFT) {
        return;
    }
    double x = 0;
    double y = 0;
    glfwGetCursorPos(surface->window, &x, &y);

    Vec2i pt(static_cast<int>(x), static_cast<int>(y));
    pt.y = surface->size.y - pt.y;

    TouchEvent event;
    event.pt = pt;
    event.eventT = TimePoint::GetNowTime();

    if(action == GLFW_PRESS) {
        event.actionType = EActionType::Press;
        ET_QueueEvent(&ETInputEvents::ET_onTouch, event);
    } else if(action == GLFW_RELEASE) {
        event.actionType = EActionType::Release;
        ET_QueueEvent(&ETInputEvents::ET_onTouch, event);
    }
}

void GLFWSurface::SetFramebufferSizeCallback(GLFWwindow* window, int w, int h) {
    auto surface = GLFW->getActiveSurface(window);
    if(!surface) {
        return;
    }
    Vec2i newSize(w, h);
    int iconified = glfwGetWindowAttrib(window, GLFW_ICONIFIED);
    if(iconified) {
        if(newSize == Vec2i(0) && surface->size != Vec2i(0)) {
            return;
        }
    }
    surface->size = Vec2i(w, h);
    ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceResized, surface->size);
}

void GLFWSurface::SetKeyboardButtonCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;

    auto surface = GLFW->getActiveSurface(window);
    if(!surface) {
        return;
    }
    if(key != GLFW_KEY_ESCAPE) {
        return;
    }
    ButtonEvent event;
    event.buttonId = EButtonId::Back;
    event.eventT = TimePoint::GetNowTime();
    if(action == GLFW_PRESS) {
        event.actionType = EActionType::Press;
        ET_QueueEvent(&ETInputEvents::ET_onButton, event);
    } else if(action == GLFW_RELEASE) {
        event.actionType = EActionType::Release;
        ET_QueueEvent(&ETInputEvents::ET_onButton, event);
    }
}

void GLFWSurface::SetIconfyCallback(GLFWwindow* window, int inconified) {
    auto surface = GLFW->getActiveSurface(window);
    if(!surface) {
        return;
    }
    if(inconified) {
        ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceHidden);
    } else {
        ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceShown);
    }
}