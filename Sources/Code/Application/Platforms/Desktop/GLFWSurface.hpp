#ifndef __GLFWS_SURFACE_HPP__
#define __GLFWS_SURFACE_HPP__

#include <memory>

#include "Platforms/Surface.hpp"

struct GlfwInit;
struct GLFWwindow;

class GLFWSurface : public Surface {
public:

    GLFWSurface();
    ~GLFWSurface();

    bool init();
    bool shouldRun();
    void update();
    void close();

    int getHeight() const override;
    int getWidth() const override;

private:

    static std::unique_ptr<GlfwInit> GLFW;

private:

    GLFWwindow* window;
    
};

#endif /* __GLFWS_SURFACE_HPP__ */
