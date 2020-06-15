#ifndef __GLFWS_SURFACE_HPP__
#define __GLFWS_SURFACE_HPP__

#include "Core/ETPrimitives.hpp"
#include "Core/ETTimer.hpp"
#include "Core/ETSurface.hpp"
#include "Core/SystemLogic.hpp"

#include <memory>
#include <vector>

class GlfwLibInitData;
struct GLFWwindow;

class GLFWSurface : public SystemLogic,
    public ETNode<ETSurface>,
    public ETNode<ETSystemTimerEvents> {
public:

    GLFWSurface();
    virtual ~GLFWSurface();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETSurface
    bool ET_show() override;
    bool ET_hide() override;
    void ET_close() override;
    void ET_swapBuffers() override;
    Vec2i ET_getSize() const override;
    GLContextType ET_getGLContextType() const override;
    bool ET_isVisible() const override;
    bool ET_isValid() const override;

    // ETSystemTimerEvents
    void ET_onSystemTick(float dt) override;

    const GLFWwindow* getWindow() const;

private:

    static void SetCursorePosCallback(GLFWwindow* window, double x, double y);
    static void SetFramebufferSizeCallback(GLFWwindow* window, int w, int h);
    static void SetMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void SetKeyboardButtonCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void SetIconfyCallback(GLFWwindow* window, int iconified);

private:

    static std::unique_ptr<GlfwLibInitData> GLFW;

private:

    GLFWwindow* window;
    Vec2i size;
    std::vector<Vec2i> activeGesture;
};

#endif /* __GLFWS_SURFACE_HPP__ */
