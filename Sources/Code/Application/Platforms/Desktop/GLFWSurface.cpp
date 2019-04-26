#include "Platforms/Desktop/GLFWSurface.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Platforms/OpenGL.hpp"

#include <GLFW/glfw3.h>

#include <cassert>

namespace {
    const int DEF_WIDTH = 600;
    const int DEF_HEIGHT = 480;
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

    GLFWSurface* getActiveSurface() {
        return activeSurface;
    }

private:

    GLFWSurface* activeSurface;
    bool status;
};

std::unique_ptr<GlfwLibInitData> GLFWSurface::GLFW = nullptr;

GLFWSurface::GLFWSurface() :
    window(nullptr),
    size(0) {
}

GLFWSurface::~GLFWSurface() {
    if(window) {
        glfwDestroyWindow(window);
    }
    GLFW->resetActiveSurface(this);
    window = nullptr;
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
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
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

    glfwMakeContextCurrent(window);
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

    ETNode<ETSurface>::connect(getEntityId());

    return true;
}

void GLFWSurface::deinit() {
    ETNode<ETSurface>::disconnect();
}

/*
void GLFWSurface::onUpdate(float dt) {
    glfwPollEvents();
}

bool GLFWSurface::onShouldRun() {
    return window != nullptr && !glfwWindowShouldClose(window);
}
*/

bool GLFWSurface::ET_isVisible() const {
    if(window) {
        return glfwGetWindowAttrib(window, GLFW_VISIBLE);
    }
    return false;
}

bool GLFWSurface::ET_show() {
    if(window) {
        glfwShowWindow(window);
        glfwFocusWindow(window);
        return true;
    }
    return false;
}

bool GLFWSurface::ET_hide() {
    if(window) {
        glfwHideWindow(window);
        return true;
    }
    return false;
}

void GLFWSurface::ET_terminate() {
    if(window) {
        glfwSetWindowShouldClose(window, true);
    }
}

void GLFWSurface::ET_swapBuffers() {
    if(window) {
        glfwSwapBuffers(window);
    }
}

Vec2i GLFWSurface::ET_getSize() const {
    return size;
}

GLContextType GLFWSurface::ET_getGLContextType() const {
    if(window) {
        return GLContextType::ES30;
    }
    return GLContextType::None;
}

void GLFWSurface::SetCursorePosCallback(GLFWwindow* window, double x, double y) {
    auto surface = GLFW->getActiveSurface();
    if(!surface) {
        LogError("[GLFWSurface::SetCursorePosCallbac] No active surface");
        return;
    }
    auto& activeGesture = surface->activeGesture;
    if(activeGesture.empty()) {
        return;
    }
    Vec2i pt(static_cast<int>(x), static_cast<int>(y));
    pt.y = surface->size.y - pt.y;
    activeGesture.push_back(pt);
    ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceTouch, ETouchType::Move, pt);
}

void GLFWSurface::SetMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    auto surface = GLFW->getActiveSurface();
    if(!surface) {
        LogError("[GLFWSurface::SetMouseButtonCallback] No active surface");
        return;
    }
    if(button != GLFW_MOUSE_BUTTON_LEFT) {
        return;
    }
    double x = 0;
    double y = 0;
    glfwGetCursorPos(window, &x, &y);
    auto& activeGesture = surface->activeGesture;

    Vec2i pt(static_cast<int>(x), static_cast<int>(y));
    pt.y = surface->size.y - pt.y;

    if(action == GLFW_PRESS) {
        activeGesture.push_back(pt);
        ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceTouch, ETouchType::Press, pt);
    } else if (action == GLFW_RELEASE) {
        activeGesture.clear();
        ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceTouch, ETouchType::Release, pt);
    }
}

void GLFWSurface::SetFramebufferSizeCallback(GLFWwindow* windwos, int w, int h) {
    auto surface = GLFW->getActiveSurface();
    if(!surface) {
        LogError("[GLFWSurface::SetFramebufferSizeCallback] No active surface");
        return;
    }
    surface->size = Vec2i(w, h);
    ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceResize, surface->size);
}