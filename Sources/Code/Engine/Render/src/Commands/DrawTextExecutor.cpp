#include "Commands/DrawTextExecutor.hpp"
#include "Render/ETRenderManager.hpp"
#include "RenderFont.hpp"

#include <cassert>

namespace {

int genTextLineVertexData(const std::string& text, const RenderFont& font,
    const Vec2& pt, size_t startIdx, size_t endIdx, Vec4* out) {

    int vertShift = 0;
    Vec2 drawPt = pt;

    for(size_t i = startIdx; i < endIdx; ++i) {
        auto ch = text[i];
        assert(ch != '\n' && "Invalid new line character");
        auto glyph = font.getGlyph(ch);
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
        out[vertShift + 0] = { glyphPt.x    , glyphPt.y + h, txBot.x, txBot.y };
        out[vertShift + 1] = { glyphPt.x + w, glyphPt.y    , txTop.x, txTop.y };
        out[vertShift + 2] = { glyphPt.x    , glyphPt.y    , txBot.x, txTop.y };
        // Second tri
        out[vertShift + 3] = { glyphPt.x    , glyphPt.y + h, txBot.x, txBot.y };
        out[vertShift + 4] = { glyphPt.x + w, glyphPt.y + h, txTop.x, txBot.y };
        out[vertShift + 5] = { glyphPt.x + w, glyphPt.y    , txTop.x, txTop.y };

        vertShift += 6;

        if(i == startIdx) {
            drawPt.x += glyph->advance.x - glyph->bearing.x;
        } else {
            drawPt.x += glyph->advance.x;
        }
        drawPt.y += glyph->advance.y;
    }

    return vertShift;
}

int calcGlyphCount(const DrawTextCmd& cmd) {
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

int DrawTextExecutor::GenTextVertData(const std::string& text, const TextMetric& metric, const RenderFont& font, bool alignAtCenter, void* out) {
    if(!out) {
        assert(false && "Invalid buffer");
        return 0;
    }

    int vertShift = 0;
    Vec4* outPtr = static_cast<Vec4*>(out);

    float xStart = 0.f;
    float yStart = metric.size.y / 2.f;
    if(alignAtCenter) {
        xStart = -metric.size.x / 2.f;
    }
    Vec2 drawPt(0.f, yStart);
    for(size_t i = 0, sz = metric.lineMetrics.size(); i < sz; ++i) {
        const auto& lineMetric = metric.lineMetrics[i];
        if(i == 0) {
            drawPt.y -= metric.firstLineOffset;
        } else {
            drawPt.y -= font.getHeight() * RenderUtils::TextNewLineOffset;
        }
        drawPt.x = xStart;
        vertShift += genTextLineVertexData(text, font, drawPt, lineMetric.startIdx, lineMetric.endIdx, outPtr + vertShift);
    }

    return vertShift;
}

DrawTextExecutor::DrawTextExecutor() {
}

DrawTextExecutor::~DrawTextExecutor() {
}

bool DrawTextExecutor::init() {
    ET_SendEventReturn(shader, &ETRenderShaderManager::ET_createShader, "text_solid_color");
    if(!shader) {
        return false;
    }
    ET_SendEventReturn(geom, &ETRenderGeometryManager::ET_createGeometry, EPrimitiveGeometryType::Vec2_Tex);
    if(!geom) {
        return false;
    }
    return true;
}

void DrawTextExecutor::deinit() {
}

void DrawTextExecutor::preDraw(RenderState& renderState) {
    DrawCommandExecutor<DrawTextCmd>::preDraw(renderState);

    if(!Core::EnumFlagsBitAND(events, EDrawCmdEventType::UpdateVertexData)) {
        return;
    }

    int totalGlyphCount = 0;
    for(size_t i = 0; i < visibleCount; ++i) {
        auto& cmd = *queue[i];
        totalGlyphCount += calcGlyphCount(cmd);
    }

    if(totalGlyphCount == 0) {
        return;
    }

    const int buffSize = sizeof(Vert_Vec2_Tex) * totalGlyphCount * 6;
    auto& buff = renderState.buffer;
    buff.resize(buffSize);

    Vec4* outPtr = static_cast<Vec4*>(buff.getWriteData());
    int vertShift = 0;

    for(size_t i = 0; i < visibleCount; ++i) {
        auto& cmd = *queue[i];
        cmd.vertStart = vertShift;
        vertShift += GenTextVertData(cmd.text, cmd.textMetric, *cmd.font, cmd.alignAtCenter, outPtr + vertShift);
        cmd.vertEnd = vertShift;
    }

    geom->setVboData(outPtr, buffSize);
}

void DrawTextExecutor::draw(RenderState& renderState, DrawCmdSlice& slice) {
    geom->bind();
    shader->bind();
    shader->setUniformMat4(UniformType::CameraMat, renderState.proj2dMat);

    const DrawTextCmd* prevCmd = nullptr;
    for(size_t i = slice.startIdx; i < slice.endIdx; ++i) {
        auto& cmd = *queue[i];
        renderState.startCommand(cmd);

        shader->setUniformMat4(UniformType::ModelMat, cmd.modelMat);

        if(!prevCmd || prevCmd->font.get() != cmd.font.get()) {
            shader->setTexture2d(UniformType::Texture, 0, *cmd.font->getFontAtlas());
        }

        shader->setUniform4f(UniformType::Color, ColorB(cmd.color.r, cmd.color.g, cmd.color.b, static_cast<uint8_t>(cmd.color.a * cmd.alpha)));

        geom->drawTriangles(cmd.vertStart, cmd.vertEnd);

        prevCmd = &cmd;
    }

    shader->unbind();
    geom->unbind();
}