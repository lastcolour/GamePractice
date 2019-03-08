#ifndef __GAME_BOARD_LOGIC_HPP__
#define __GAME_BOARD_LOGIC_HPP__

#include "Game/GameLogic.hpp"
#include "Math/AABB.hpp"
#include "ETApplicationInterfaces.hpp"

#include <vector>
#include <memory>

class GameObject;

class GameBoardLogic : public GameLogic,
    public ETNode<ETSurfaceEvents> {
public:

    GameBoardLogic();
    virtual ~GameBoardLogic();

    bool init(const JSONNode& node) override;

    // ETSurfaceEvents
    void ET_onSurfaceTouch(ETouchType touchType, const Vec2i& pt) override;

private:

    bool serialize(const JSONNode& node);

private:

    EntityId findTouchedEntity(const Vec2i& pt) const;

private:

    Vec2i boardSize;
    float space;
    float cellScale;
    std::string cellObject;

    struct TouchAaabb {
        AABB2Di box;
        EntityId entId;
    };

    std::vector<TouchAaabb> touchMap;
    EntityId activeTouchEntId;
};

#endif /* __GAME_BOARD_LOGIC_HPP__ */