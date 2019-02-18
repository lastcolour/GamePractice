#ifndef __GLFWS_SURFACE_HPP__
#define __GLFWS_SURFACE_HPP__

#include <memory>

#include "Surface.hpp"

class GlfwLibInitData;
struct GLFWwindow;

class GLFWSurface : public Surface {
public:

    GLFWSurface();
    ~GLFWSurface();

    bool init() override;
    bool show() override;
    bool hide() override;
    bool shouldRun() override;
    void update() override;
    void terminate() override;
    void swapBuffers() override;

    int getHeight() const override;
    int getWidth() const override;

private:

    static void SetFramebufferSizeCallback(GLFWwindow* windwos, int w, int h);

private:

    static std::unique_ptr<GlfwLibInitData> GLFW;

private:

    GLFWwindow* window;
    int width;
    int height;
};

#endif /* __GLFWS_SURFACE_HPP__ */
