#ifndef __GAME_OBJECT_HPP__
#define __GAME_OBJECT_HPP__

#include "Math/Transform.hpp"

#include <string>
#include <memory>
#include <vector>

class RenderLogic;

class GameObject {
public:

    GameObject(const std::string& objectName);
    ~GameObject();

    void update();

    void addRender(std::unique_ptr<RenderLogic>&& renderLogic);

    RenderLogic* getRender();
    const std::string& getName() const;

private:

    std::string name;
    Math::Transform tm;
    std::unique_ptr<RenderLogic> render;
};

#endif /* __GAME_OBJECT_HPP__ */