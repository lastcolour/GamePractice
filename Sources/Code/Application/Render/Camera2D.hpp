#ifndef __CAMERA_2D_HPP__
#define __CAMERA_2D_HPP__

#include "Math/Matrix.hpp"

class Camera2D {
public:

    Camera2D();
    ~Camera2D();

    void setViewport(int w, int h);
    const Vec2i& getViewport() const;
    const Mat4& getProjMat4() const;

private:

    Camera2D(const Camera2D&) = delete;
    Camera2D& operator=(const Camera2D&) = delete;

private:

    Mat4 projection;
    Vec2i viewport;
};

#endif /* __CAMERA_2D_HPP__ */