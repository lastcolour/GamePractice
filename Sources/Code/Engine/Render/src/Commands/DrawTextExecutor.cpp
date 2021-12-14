#include "Commands/DrawTextExecutor.hpp"
#include "Render/ETRenderManager.hpp"
#include "RenderFont.hpp"
#include "RenderUtils.hpp"

#include <cassert>

namespace {

void genTextLineVertexData(const DrawTextCmd& cmd, std::vector<Vec4>& vertData, int& vertShift,
    const Vec2& pt, size_t startIdx, size_t endIdx) {

    Vec2 drawPt = pt;
    for(size_t i = startIdx; i < endIdx; ++i) {
        auto ch = cmd.text[i];
        assert(ch != '\n' && "Invalid new line character");
        auto glyph = cmd.font->getGlyph(ch);
        if(!glyph) {
            continue;
        }

        Vec2 glyphPt(0);
        glyphPt.x = drawPt.x;
        if(i > startIdx) {
            glyphPt.x += glyph->bearing.x;
        }
        glyphPt.y = drawPt.y - (glyph->size.y - glyph->bearing.y);
        const float w = static_cast<float>(glyph->size.x);
        const float h = static_cast<float>(glyph->size.y);

        const auto& txBot = glyph->texCoords.bot;
        const auto& txTop = glyph->texCoords.top;

        // First tri
        vertData[vertShift + 0] = { glyphPt.x    , glyphPt.y + h, txBot.x, txBot.y };
        vertData[vertShift + 1] = { glyphPt.x + w, glyphPt.y    , txTop.x, txTop.y };
        vertData[vertShift + 2] = { glyphPt.x    , glyphPt.y    , txBot.x, txTop.y };
        // Second tri
        vertData[vertShift + 3] = { glyphPt.x    , glyphPt.y + h, txBot.x, txBot.y };
        vertData[vertShift + 4] = { glyphPt.x + w, glyphPt.y + h, txTop.x, txBot.y };
        vertData[vertShift + 5] = { glyphPt.x + w, glyphPt.y    , txTop.x, txTop.y };

        vertShift += 6;

        if(i == startIdx) {
            drawPt.x += glyph->advance.x - glyph->bearing.x;
        } else {
            drawPt.x += glyph->advance.x;
        }
        drawPt.y += glyph->advance.y;
    }
}

void genTextVertexData(const DrawTextCmd& cmd, std::vector<Vec4>& vertData, int& vertShift) {
    float xStart = 0.f;
    float yStart = cmd.textMetric.size.y / 2.f;
    if(cmd.alignAtCenter) {
        xStart = -cmd.textMetric.size.x / 2.f;
    }
    Vec2 drawPt(0.f, yStart);
    for(size_t i = 0, sz = cmd.textMetric.lineMetrics.size(); i < sz; ++i) {
        const auto& lineMetric = cmd.textMetric.lineMetrics[i];
        if(i == 0) {
            drawPt.y -= cmd.textMetric.firstLineOffset;
        } else {
            drawPt.y -= cmd.font->getHeight() * RenderUtils::TextNewLineOffset;
        }
        drawPt.x = xStart;
        genTextLineVertexData(cmd, vertData, vertShift,
            drawPt, lineMetric.startIdx, lineMetric.endIdx);
    }
}

int calcVertCount(const DrawTextCmd& cmd) {
    int vertCount = 0;
    for(auto& ch : cmd.text) {
        if(cmd.font->getGlyph(ch)) {
            vertCount += 1;
        }
    }
    if(vertCount == 0 && cmd.text.empty()) {
        assert(false && "Empty text");
    }
    return vertCount;
}

} // namespace

DrawTextExecutor::DrawTextExecutor() {
}

DrawTextExecutor::~DrawTextExecutor() {
}

bool DrawTextExecutor::init() {
    ET_SendEventReturn(shader, &ETRenderShaderManager::ET_createShader, "text_solid_color");
    if(!shader) {
        return false;
    }
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Text);
    if(!geom) {
        return false;
    }
    return true;
}

void DrawTextExecutor::deinit() {
}

void DrawTextExecutor::preDraw() {
    if(Core::EnumFlagsBitAND(events, EDrawCmdEventType::Reorder)) {
        visibleCount = DrawCmdUtils::SortCmdDrawQueue(queue);
        addEvent(EDrawCmdEventType::UpdateVertexData);
    }
    if(Core::EnumFlagsBitAND(events, EDrawCmdEventType::UpdateVertexData)) {
        int totalVertCount = 0;
        for(size_t i = 0; i < visibleCount; ++i) {
            auto& cmd = *queue[i];
            totalVertCount += calcVertCount(cmd);
        }

        vertData.resize(totalVertCount * 6);
        int vertShift = 0;

        for(size_t i = 0; i < visibleCount; ++i) {
            auto& cmd = *queue[i];
            cmd.vertStart = vertShift;
            genTextVertexData(*queue[i], vertData, vertShift);
            cmd.vertEnd = vertShift;
        }

        geom->bind();
        geom->vboData(&vertData[0], sizeof(Vec4) * vertData.size());
        geom->unbind();

        vertData.clear();
    }
}

void DrawTextExecutor::draw(RenderState& renderState, DrawCmdSlice& slice) {
    geom->bind();
    shader->bind();
    shader->setUniformMat4(UniformType::CameraMat, renderState.proj2dMat);

    RenderTexture* prevFontAtlas = nullptr;
    for(size_t i = slice.startIdx; i < slice.endIdx; ++i) {
        auto& cmd = *queue[i];
        renderState.startCommand(cmd);

        auto fontAtlas = cmd.font->getFontAtlas();
        if(fontAtlas != prevFontAtlas) {
            shader->setTexture2d(UniformType::Texture, 0, *fontAtlas);
        }
        shader->setUniformMat4(UniformType::ModelMat, cmd.modelMat);
        // shader->setUniform4f(UniformType::Color,
        //     ColorB(cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a * cmd.alpha));

        geom->drawTriangles(cmd.vertStart, cmd.vertEnd);
    }

    shader->unbind();
    geom->unbind();
}