#include "Render/RenderLogic.hpp"

#include "Render/Render.hpp"
#include "Render/RenderGeometry.hpp"
#include "Render/RenderMaterial.hpp"
#include "Environment.hpp"

#include "JSONNode.hpp"

#include <cassert>

bool RenderLogic::init(const JSONNode& node) {
    std::string matName;
    std::string geomName;
    node.value("mat", matName);
    node.value("geom", geomName);
    auto render = GetEnv()->getRender();
    if(!render) {
        return false;
    }
    geom = render->createGeometry(geomName);
    if(!geom) {
        return false;
    }
    mat = render->createMaterial(matName);
    if(!mat) {
        return false;
    }
    return true;
}

void RenderLogic::update() {
    mat->bind();
    geom->draw();
    mat->unbind();
}

RenderMaterial& RenderLogic::getMaterila() {
    assert(mat && "Invalid material");
    return *mat.get();
}