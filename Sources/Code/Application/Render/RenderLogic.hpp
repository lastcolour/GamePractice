#ifndef __RENDER_LOGIC_HPP__
#define __RENDER_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "Platforms/OpenGL.hpp"
#include "Math/Transform.hpp"
#include "Render/Color.hpp"

#include <string>
#include <memory>

class RenderMaterial;
class RenderGeometry;

class RenderLogic : public GameLogic {
public:

    bool init(const JSONNode& node) override;
    void update() override;

    RenderMaterial& getMaterila();

private:

    std::shared_ptr<RenderMaterial> mat;
    std::shared_ptr<RenderGeometry> geom;
};

#endif /* __RENDER_LOGIC_HPP__ */