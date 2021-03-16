#ifndef __UI_ADDITIVE_TRANSFORM_HPP__
#define __UI_ADDITIVE_TRANSFORM_HPP__

struct AddtiveUITransform {
public:

    AddtiveUITransform() :
        alpha(1.f) {}

    void reset() {
        tm = Transform();
        alpha = 1.f;
    }

    void combine(const AddtiveUITransform& other) {
        alpha *= other.alpha;
        tm.pt += other.tm.pt;
        tm.scale = other.tm.scale.getScaled(tm.scale);
    }

    Transform applyTm(const Transform& targetTm) {
        Transform resTm;
        resTm.pt = targetTm.pt + tm.pt;
        resTm.scale = targetTm.scale.getScaled(tm.scale);
        return resTm;
    }

    float applyAlpha(float targetAlpha) {
        return alpha * targetAlpha;
    }

    void inverse() {
        tm.pt = -tm.pt;
        tm.scale = Vec3(1.f / tm.scale.x, 1.f / tm.scale.y, 1.f);
        alpha = 1.f / alpha;
    }

public:

    Transform tm;
    float alpha;
};

#endif /* __UI_ADDITIotherVE_TRANSFORM_HPP__ */