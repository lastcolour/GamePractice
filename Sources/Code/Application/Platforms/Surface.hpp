#ifndef __SURFACE_HPP__
#define __SURFACE_HPP__

class Surface {
public:

    virtual ~Surface() {}

    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;
};

#endif /* __SURFACE_HPP__ */