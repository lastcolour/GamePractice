#ifndef __ET_RENDER_PROXY_NODE_HPP__
#define __ET_RENDER_PROXY_NODE_HPP__

#include "Render/Color.hpp"
#include "Math/Transform.hpp"

#include <string>

struct ETRenderProxyNode {
    virtual ~ETRenderProxyNode() = default;
    virtual void ET_setAlpha(float newAlpha) = 0;
    virtual void ET_setDrawPriority(int newDrawPriority) = 0;
    virtual void ET_setVisible(bool flag) = 0;
    virtual void ET_setColor0(const ColorB& newColor) = 0;
    virtual void ET_setColor1(const ColorB& newColor) = 0;
    virtual void ET_setFontHeight(int newHeight) = 0;
    virtual void ET_setSize(const Vec2i& newSize) = 0;
    virtual void ET_setText(const std::string& newText) = 0;
    virtual void ET_setImage(const std::string& newImage) = 0;
    virtual void ET_setVertical(bool flag) = 0;
    virtual void ET_setTransform(const Transform& newTm) = 0;
};

struct ETRenderProxyNodeEvents {
    virtual ~ETRenderProxyNodeEvents() = default;
    virtual void ET_syncWithRender() = 0;
};

#endif /* __ET_RENDER_PROXY_NODE_HPP__ */