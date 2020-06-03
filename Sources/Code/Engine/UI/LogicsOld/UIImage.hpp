#ifndef __UI_IMAGER_HPP_
#define __UI_IMAGER_HPP_

#include "UI/Logics/UIBaseBox.hpp"

class UIImage : public UIBaseBox,
    public ETNode<ETUIImage> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIImage();
    virtual ~UIImage();

    // UIBaseBox
    bool init() override;
    void deinit() override;

    // UIImage
    void ET_setImage(const char* image) override;

protected:

    Vec2i calculateBoxSize(const AABB2Di& parentBox) const override;
    EntityId getRenderId() const override;

private:

    void createRenderer();

private:

    EntityId renderId;
    std::string image;
};

#endif /* __UI_IMAGER_HPP_ */