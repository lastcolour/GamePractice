#ifndef __COMMANDS_HPP__
#define __COMMANDS_HPP__

#include "Render/RenderCommon.hpp"
#include "Particles/ParticlesEmittersPool.hpp"
#include "RenderFont.hpp"

class RenderTexture;

enum class EDrawCmdType {
    Text = 0,
    Quad,
    TexturedQuad,
    Particles,
    Blur,
    COUNT
};

class DrawCmd {
public:

    static bool IsVisible(const DrawCmd& cmd) {
        if(cmd.alpha <= 0.001f && cmd.stencilData.opType == EStencilOpType::Disabled) {
            return false;
        }
        return cmd.visible;
    }

    static void QueueToRemove(DrawCmd& drawCmd, EDrawCmdType cmdType);
    static void QueueToRender(DrawCmd& drawCmd, EDrawCmdType cmdType);
    static void QueueTMUpdate(DrawCmd& drawCmd, const Transform& newTm, EDrawCmdType cmdType);
    static void QueueVisUpdate(DrawCmd& drawCmd, bool newVisible, EDrawCmdType cmdType);
    static void QueueZIndexUpdate(DrawCmd& drawCmd, int newZIndex, EDrawCmdType cmdType);
    static void QueueAlphaUpdate(DrawCmd& drawCmd, float newAlpha, EDrawCmdType cmdType);
    static void QueueScaleUpdate(DrawCmd& drawCmd, const Vec3& newScale, EDrawCmdType cmdType);
    static void QueueStencilDataUpdate(DrawCmd& drawCmd, const StencilWirteReadData& newStencilData, EDrawCmdType cmdType);
    static void QueueSizeUpdate(DrawCmd& drawCmd, const Vec2& prevSize, const Vec2& newSize, EDrawCmdType cmdType);

public:

    DrawCmd() :
        modelMat(1.f),
        alpha(1.f),
        zIndex(0),
        visible(false) {}

public:

    EntityId refEntityId;
    Mat4 modelMat;
    StencilWirteReadData stencilData;
    BlendOpPair blendOpPair;
    float alpha;
    int zIndex;
    bool visible;
};

class DrawTextCmd : public DrawCmd {
public:

    static bool IsVisible(const DrawTextCmd& cmd) {
        if(cmd.text.empty()) {
            return false;
        }
        if(!cmd.font || !cmd.font->getFontAtlas()) {
            return false;
        }
        return DrawCmd::IsVisible(cmd);
    }

    static void QueueTextUpdate(DrawCmd& drawCmd, const std::string& newText);
    static void QueueColorUpdate(DrawCmd& drawCmd, const ColorB& newColor);
    static void QueueFontHeightUpdate(DrawCmd& drawCmd, float newFontHeight);
    static void QueueFontTypeUpdate(DrawCmd& drawCmd, EFontType newFontType);
    static void QueueAlignUpdate(DrawCmd& drawCmd, bool newAlign);

public:

    DrawTextCmd() :
        color(255, 255, 255),
        fontHeight(24.f),
        vertStart(-1),
        vertEnd(-1),
        alignAtCenter(false) {}

public:

    std::string text;
    std::shared_ptr<RenderFont> font;
    TextMetric textMetric;
    ColorB color;
    float fontHeight;
    int vertStart;
    int vertEnd;
    bool alignAtCenter;
};

class DrawColoredQuadCmd : public DrawCmd {
public:

    static bool IsVisible(const DrawColoredQuadCmd& cmd) {
        return DrawCmd::IsVisible(cmd);
    }

    static void QueueColorUpdate(DrawCmd& drawCmd, const ColorB& newColor);

public:

    DrawColoredQuadCmd() :
        color(255, 255, 255) {}

public:

    ColorB color;
};

class DrawTexturedQuadCmd : public DrawCmd {
public:

    enum class EImageCmdType {
        Image = 0,
        Gradient,
        NinePatch
    };

public:

    static bool IsVisible(const DrawTexturedQuadCmd& cmd) {
        if(!cmd.texObj) {
            return false;
        }
        return DrawCmd::IsVisible(cmd);
    }

    static void QueueTexInfoUpdate(DrawCmd& drawCmd, const TextureInfo& newTexInfo);
    static void QueueNinePatchUpdate(DrawCmd& drawCmd, const Vec2& newPatches);
    static void QueueSetTexGradient(DrawCmd& drawCmd, const ColorB& startCol, const ColorB& endCol, bool isVertical);

public:

    DrawTexturedQuadCmd() :
        ninePatches(0.3f, 0.3f),
        patchesScale(1.f),
        vertStart(-1),
        vertEnd(-1),
        imageType(EImageCmdType::Image) {}

public:

    TextureInfo texInfo;
    std::shared_ptr<RenderTexture> texObj;
    Vec2 ninePatches;
    float patchesScale;
    int vertStart;
    int vertEnd;
    EImageCmdType imageType;
};

class DrawParticlesCmd : public DrawCmd {
public:

    static bool IsVisible(const DrawParticlesCmd& cmd) {
        if(!cmd.texObj) {
            return false;
        }
        return DrawCmd::IsVisible(cmd);
    }

    static void QueueColorConfigUpdate(DrawCmd& drawCmd, const ParticlesEmitterColorConfig& newColorConf);
    static void QueueMovementConfigUpdate(DrawCmd& drawCmd, const ParticlesEmitterMovementConfig& newMovementConf);
    static void QueueEmissionConfigUpdate(DrawCmd& drawCmd, const ParticlesEmitterEmissionConfig& newEmissionConf);
    static void QueueRenderConfigUpdate(DrawCmd& drawCmd, const ParticlesEmitterRenderConfig& newRenderConf);
    static void QueueSubEmittersConfigUpdate(DrawCmd& drawCmd, const ParticlesSubEmittersConfig& newSubEmittersConf);
    static void QueueSizeConfigUpdate(DrawCmd& drawCmd, const ParticlesEmitterSizeConfig& newSizeConf);
    static void QueueCreateEmitterUpdate(DrawCmd& drawCmd, const EmitRequest& emitReq);
    static void QueueStopTrackedEmitterUpate(DrawCmd& drawCmd, EntityId trackEntId);

public:

    std::shared_ptr<RenderTexture> texObj;
    ParticlesEmitterRenderConfig renderConfig;
    ParticlesEmittersPool emittersPool;
};

class DrawBlurCmd : public DrawCmd {
public:

    static bool IsVisible(const DrawBlurCmd& cmd) {
        return DrawCmd::IsVisible(cmd);
    }

public:

    DrawBlurCmd() :
        passes(1),
        downScale(2) {}

public:

    int passes;
    int downScale;
};

class DebugDrawLineCmd {
public:

    DebugDrawLineCmd() :
        startPt(0.f),
        startCol(255, 255, 255),
        endPt(0.f),
        endCol(255, 255, 255) {}

public:

    Vec2 startPt;
    ColorB startCol;
    Vec2 endPt;
    ColorB endCol;
};

class DebugDrawQuadCmd {
public:

    DebugDrawQuadCmd() :
        box(0.f),
        col(255, 255, 255) {}

public:

    AABB2D box;
    ColorB col;
};

class DebugDrawTextCmd {
public:

    DebugDrawTextCmd() :
        pt(0.f),
        col(255, 255, 255),
        fontHeight(24.f) {}

public:

    std::string text;
    Vec2 pt;
    ColorB col;
    float fontHeight;
};

namespace DrawCmdUtils {

Mat4 CalcModelMat(const Transform& tm, const Vec3& scale);

} // namespace DrawCmdUtils

#endif /* __COMMANDS_HPP__ */