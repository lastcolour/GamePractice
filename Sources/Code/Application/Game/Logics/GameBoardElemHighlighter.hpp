#ifndef __GAME_BOARD_ELEM_HIGHLIGHTER_HPP__
#define __GAME_BOARD_ELEM_HIGHLIGHTER_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Game/ETGameTimer.hpp"
#include "Game/ETGameBoard.hpp"

class ReflectContext;

class GameBoardElemHighlighter : public EntityLogic,
    public ETNode<ETGameTimerEvents>,
    public ETNode<ETGameBoardElemHighlighter> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameBoardElemHighlighter();
    virtual ~GameBoardElemHighlighter();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETGameTimerEvents
    void ET_onGameTick(float dt) override;

    // ETGameBoardElemHighlighter
    void ET_hightlightCell(const Vec2i& cellPt);

private:

    enum class State {
        Finished = 0,
        FadeOut
    };

    struct HighlightElem {
        Vec2i boardPt;
        EntityId entId;
        State state;
        float duration;
    };

private:

    bool createElemsPool();
    void updateElem(HighlightElem& elem, float dt);
    void destroyAllElems();

private:

    std::string hightlightEntityName;
    std::vector<HighlightElem> elements;
    float fadeOutDuration;
    float cellScale;
};

#endif /*__GAME_BOARD_ELEM_HIGHLIGHTER_HPP__ */