#ifndef __GLFWS_SURFACE_HPP__
#define __GLFWS_SURFACE_HPP__

#include "Platform.hpp"

#include <memory>
#include <vector>

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
    GLContextType ET_getGLContextType() const override;
    bool ET_isVisible() const override;

protected:

    // SystemLogic
    bool onInit() override;
    bool onShouldRun() override;
    void onUpdate() override;

private:

    static void SetCursorePosCallback(GLFWwindow* window, double x, double y);
    static void SetFramebufferSizeCallback(GLFWwindow* window, int w, int h);
    static void SetMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

private:

    static std::unique_ptr<GlfwLibInitData> GLFW;

private:

    GLFWwindow* window;
    Vec2i size;
    std::vector<Vec2i> activeGesture;
};

#endif /* __GLFWS_SURFACE_HPP__ */
