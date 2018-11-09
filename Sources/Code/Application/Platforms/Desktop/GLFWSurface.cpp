#include "Platforms/Desktop/GLFWSurface.hpp"

#include "Platforms/OpenGL.hpp"

class GlfwInit {
public:

    GlfwInit() : status(false) {}
    ~GlfwInit() { glfwTerminate(); status = false; }

    void init() { status = glfwInit(); }
    bool getStatus() { return status; }

private:

    bool status;
};

std::unique_ptr<GlfwInit> GLFWSurface::GLFW = nullptr;

GLFWSurface::GLFWSurface() : window(nullptr) {
}

GLFWSurface::~GLFWSurface() {
    if(window) {
        glfwDestroyWindow(window);
    }
    window = nullptr;
}

bool GLFWSurface::init() {
    if(!GLFW) {
        GLFW.reset(new GlfwInit);
        GLFW->init();
    }
    if(!GLFW->getStatus()) {
        return false;
    }

    window = glfwCreateWindow(640, 480, "Surface", nullptr, nullptr);
    if (!window) {
        return false;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));

    return true;
}

bool GLFWSurface::shouldRun() {
    return window != nullptr && !glfwWindowShouldClose(window);
}

void GLFWSurface::update() {
    glfwSwapBuffers(window);
    glfwPollEvents();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void GLFWSurface::close() {
    if(window) {
        glfwSetWindowShouldClose(window, true);
    }
}

int GLFWSurface::getHeight() const {
    return 480;
}

int GLFWSurface::getWidth() const {
    return 600;
}