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
        tm.addDelta(other.tm);
        alpha *= other.alpha;
    }

    void inverse() {
        tm.inverse();
        alpha = 1.f / alpha;
    }

public:

    Transform tm;
    float alpha;
};

#endif /* __UI_ADDITIotherVE_TRANSFORM_HPP__ */