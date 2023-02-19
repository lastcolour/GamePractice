#include "Commands/DebugDrawExecutor.hpp"
#include "Render/ETDebugRender.hpp"
#include "Render/ETRenderManager.hpp"
#include "Commands/DrawTextExecutor.hpp"
#include "RenderFont.hpp"
#include "RenderUtils.hpp"

DebugDrawExecutor::DebugDrawExecutor() {
}

DebugDrawExecutor::~DebugDrawExecutor() {
}

bool DebugDrawExecutor::init() {
    // init lines
    ET_SendEventReturn(lineShader, &ETRenderShaderManager::ET_createShader, "line_solid_color");
    if(!lineShader) {
        return false;
    }
    ET_SendEventReturn(lineGeom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Line);
    if(!lineGeom) {
        return false;
    }

    // init quads
    ET_SendEventReturn(quadShader, &ETRenderShaderManager::ET_createShader, "geom_solid_color");
    if(!quadShader) {
        return false;
    }
    ET_SendEventReturn(quadGeom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Vec3);
    if(!quadGeom) {
        return false;
    }

    // init texts
    ET_SendEventReturn(textShader, &ETRenderShaderManager::ET_createShader, "text_solid_color");
    if(!textShader) {
        return false;
    }
    ET_SendEventReturn(textGeom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Vec2_Tex);
    if(!textGeom) {
        return false;
    }
    ET_SendEventReturn(textFont, &ETRenderFontManager::ET_createFont, EFontType::Debug);
    if(!textFont) {
        return false;
    }

    return true;
}

void DebugDrawExecutor::deinit() {
}

void DebugDrawExecutor::preRender() {
    cmdCollector.clearCmds();
    ET_SendEvent(&ETDebugInfoProvider::ET_drawDebugInfo, cmdCollector);
}

void DebugDrawExecutor::drawQuads(RenderState& renderState) {
    quadGeom->bind();
    quadShader->bind();
    quadShader->setUniformMat4(UniformType::CameraMat, renderState.proj2dMat);

    Transform tm;
    for(auto& quadCmd : cmdCollector.drawQuadCmds) {

        const Vec2 center = quadCmd.box.getCenter();
        const Vec2 scale = quadCmd.box.getSize();
        tm.pt = Vec3(center.x, center.y, 0.f);
        tm.scale = Vec3(scale.x / 2.f, scale.y / 2.f, 1.f);

        quadShader->setUniformMat4(UniformType::ModelMat, tm.toMat4());
        quadShader->setUniform4f(UniformType::Color, quadCmd.col);
        quadGeom->drawTriangles(0, 6);
    }

    quadShader->unbind();
    quadGeom->unbind();
}

void DebugDrawExecutor::drawLines(RenderState& renderState) {
    const int lines = static_cast<int>(cmdCollector.drawLinesCmds.size());

    lineGeom->setVboData(&cmdCollector.drawLinesCmds[0], lines * sizeof(DebugDrawLineCmd));

    lineGeom->bind();
    lineShader->bind();

    lineShader->setUniformMat4(UniformType::CameraMat, renderState.proj2dMat);
    lineShader->setUniform1f(UniformType::Alpha, 1.f);

    lineGeom->drawLines(0, lines * 2);

    lineShader->unbind();
    lineGeom->unbind();
}

void DebugDrawExecutor::drawTexts(RenderState& renderState) {
    {
        int vertCount = 0;
        size_t glyphs = 0;
        for(auto& textCmd : cmdCollector.drawTextCmds) {
            glyphs += textCmd.text.size();
        }

        auto& buff = renderState.buffer;
        const size_t buffSize = glyphs * sizeof(Vert_Vec2_Tex) * 6;
        buff.resize(buffSize);
        Vert_Vec2_Tex* dataPtr = static_cast<Vert_Vec2_Tex*>(buff.getWriteData());

        const bool aligntAtCenter = false;

        for(auto& textCmd : cmdCollector.drawTextCmds) {
            TextMetric metric = textFont->getTextMetric(textCmd.text);
            textCmd.vertStart = vertCount;
            vertCount += DrawTextExecutor::GenTextVertData(textCmd.text, metric, *textFont, aligntAtCenter, dataPtr + vertCount);
            textCmd.vertEnd = vertCount;
        }

        if(vertCount == 0) {
            return;
        }

        textGeom->setVboData(dataPtr, vertCount * sizeof(Vert_Vec2_Tex));
    }

    textGeom->bind();

    textShader->bind();
    textShader->setUniformMat4(UniformType::CameraMat, renderState.proj2dMat);
    textShader->setTexture2d(UniformType::Texture, 0, *textFont->getFontAtlas());

    Transform tm;
    for(auto& textCmd : cmdCollector.drawTextCmds) {
        if(textCmd.vertEnd > textCmd.vertStart) {
            tm.pt = Vec3(textCmd.pt.x, textCmd.pt.y, 0.f);
            tm.scale = Vec3(textCmd.fontHeight / static_cast<float>(textFont->getHeight()));
            tm.scale.z = 1.f;
        
            textShader->setUniform4f(UniformType::Color, textCmd.col);
            textShader->setUniformMat4(UniformType::ModelMat, tm.toMat4());

            textGeom->drawTriangles(textCmd.vertStart, textCmd.vertEnd);
        }
    }

    textShader->unbind();
    textGeom->unbind();
}

void DebugDrawExecutor::draw(RenderState& renderState) {
    renderState.setStencilState(StencilWirteReadData{});

    const bool preMultAlpha = false;
    renderState.setBlenMode(EBlendMode::Normal, preMultAlpha);

    if(!cmdCollector.drawQuadCmds.empty()) {
        drawQuads(renderState);
    }
    if(!cmdCollector.drawLinesCmds.empty()) {
        drawLines(renderState);
    }
    if(!cmdCollector.drawTextCmds.empty()) {
        drawTexts(renderState);
    }
}