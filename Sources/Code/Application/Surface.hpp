#ifndef __SURFACE_HPP__
#define __SURFACE_HPP__

class Surface {
public:

    virtual ~Surface() = default;

    virtual bool init() = 0;
    virtual bool show() = 0;
    virtual bool hide() = 0;
    virtual bool shouldRun() = 0;
    virtual void update() = 0;
    virtual void terminate() = 0;
    virtual void swapBuffers() = 0;

    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
};

#endif /* __SURFACE_HPP__ */