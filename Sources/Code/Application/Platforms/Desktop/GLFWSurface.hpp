#ifndef __GLFWS_SURFACE_HPP__
#define __GLFWS_SURFACE_HPP__

#include <memory>

struct GlfwInit;
struct GLFWwindow;

class GLFWSurface {
public:

    GLFWSurface();
    ~GLFWSurface();

    bool init();
    bool shouldRun();
    void update();
    void close();

private:

    static std::unique_ptr<GlfwInit> GLFW;

private:

    GLFWwindow* window;
    
};

#endif /* __GLFWS_SURFACE_HPP__ */
