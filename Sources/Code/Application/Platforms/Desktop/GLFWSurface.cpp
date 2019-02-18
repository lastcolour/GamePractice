#include "Platforms/Desktop/GLFWSurface.hpp"
#include "Platforms/OpenGL.hpp"
#include "Render/Render.hpp"
#include "Logger.hpp"

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
    width(0),
    height(0) {
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

    window = glfwCreateWindow(DEF_WIDTH, DEF_HEIGHT, DEF_WINDOW_NAME, nullptr, nullptr);
    if(!window) {
        const char* errStr = nullptr;
        int errCode = glfwGetError(&errStr);
        LogError("[GLFWSurface] Can't create window: '%s' (%d)", errStr, errCode);
        return false;
    }

    width = DEF_WIDTH;
    height = DEF_HEIGHT;

    glfwSetFramebufferSizeCallback(window, SetFramebufferSizeCallback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

    return true;
}

bool GLFWSurface::show() {
    if(window) {
        glfwShowWindow(window);
        return true;
    }
    return false;
}

bool GLFWSurface::hide() {
    if(window) {
        glfwHideWindow(window);
        return true;
    }
    return false;
}

void GLFWSurface::terminate() {
    if(window) {
        glfwSetWindowShouldClose(window, true);
    }
}

bool GLFWSurface::shouldRun() {
    return window != nullptr && !glfwWindowShouldClose(window);
}

void GLFWSurface::update() {
    glfwPollEvents();
}

void GLFWSurface::swapBuffers() {
    if(window) {
        glfwSwapBuffers(window);
    }
}

void GLFWSurface::SetFramebufferSizeCallback(GLFWwindow* windwos, int w, int h) {
    auto surface = GLFW->getActiveSurface();
    if(!surface) {
        LogError("[GLFWSurface::SetFramebufferSizeCallback] No active surface");
        return;
    }
    surface->width = w;
    surface->height = h;
    if(auto render = GetEnv()->getRender()) {
        render->setViewport(w, h);
    }
}

int GLFWSurface::getHeight() const {
    return width;
}

int GLFWSurface::getWidth() const {
    return height;
}