#include "Platforms/Desktop/GLFWSurface.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Platforms/OpenGL.hpp"

#include <GLFW/glfw3.h>

#include <cassert>

namespace {
    const int DEF_WIDTH = 600;
    const int DEF_HEIGHT = 480;
    const char* DEF_WINDOW_NAME = "Game01";
}

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

bool GLFWSurface::onInit() {
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

    glfwMakeContextCurrent(window);
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

    ETNode<ETSurface>::connect(getEntityId());

    return true;
}

void GLFWSurface::onUpdate() {
    glfwPollEvents();
}

bool GLFWSurface::onShouldRun() {
    return window != nullptr && !glfwWindowShouldClose(window);
}

bool GLFWSurface::ET_show() {
    if(window) {
        glfwShowWindow(window);
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

bool GLFWSurface::ET_canRender() const {
    return window != nullptr;
}

void GLFWSurface::SetMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    double x = 0;
    double y = 0;
    glfwGetCursorPos(window, &x, &y);

    ET_SendEventToAll(&ETSurfaceEvents::ET_onSurfaceTouch,
        Vec2i(static_cast<int>(x), static_cast<int>(y)));
}

void GLFWSurface::SetFramebufferSizeCallback(GLFWwindow* windwos, int w, int h) {
    auto surface = GLFW->getActiveSurface();
    if(!surface) {
        LogError("[GLFWSurface::SetFramebufferSizeCallback] No active surface");
        return;
    }
    surface->size = Vec2i(w, h);
    ET_SendEventToAll(&ETSurfaceEvents::ET_onSurfaceResize, surface->size);
}