#ifndef __VERT_TYPES_HPP__
#define __VERT_TYPES_HPP__

struct Vert_Vec2_Tex {
    Vec2 pt;
    Vec2 uv;
};

struct Vert_Vec3_Tex {
    Vec3 pt;
    Vec2 uv;
};

struct Vert_Vec3 {
    Vec3 pt;
};

template<typename T>
struct VertQuad {

    using VertType = T;
    
public:

    T botLeft;
    T botRight;
    T topLeft;
    T topRight;
};

template<typename QuadType>
int CreateQuadVertData(const QuadType& quad, typename QuadType::VertType* out) {
    // first tri
    out[0] = quad.botLeft;
    out[1] = quad.topLeft;
    out[2] = quad.botRight;

    // second tri
    out[3] = quad.topLeft;
    out[4] = quad.topRight;
    out[5] = quad.botRight;
    return 6;
}

#endif /* __VERT_TYPES_HPP__ */