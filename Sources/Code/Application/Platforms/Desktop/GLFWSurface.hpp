#ifndef __GLFWS_SURFACE_HPP__
#define __GLFWS_SURFACE_HPP__

#include "Platform.hpp"

#include <memory>

class GlfwLibInitData;
struct GLFWwindow;

class GLFWSurface : public Surface {
public:

    GLFWSurface();
    virtual ~GLFWSurface();

    // ETSurface
    bool ET_show() override;
    bool ET_hide() override;
    void ET_terminate() override;
    void ET_swapBuffers() override;
    Vec2i ET_getSize() const override;
    bool ET_canRender() const override;

protected:

    // SystemLogic
    bool onInit() override;
    bool onShouldRun() override;
    void onUpdate() override;

private:

    static void SetFramebufferSizeCallback(GLFWwindow* window, int w, int h);
    static void SetMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

private:

    static std::unique_ptr<GlfwLibInitData> GLFW;

private:

    GLFWwindow* window;
    Vec2i size;
};

#endif /* __GLFWS_SURFACE_HPP__ */
