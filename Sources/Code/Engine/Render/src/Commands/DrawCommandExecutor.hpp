#ifndef __DRAW_COMMAND_EXECUTOR_HPP__
#define __DRAW_COMMAND_EXECUTOR_HPP__

#include "Commands/RenderState.hpp"
#include "Commands/ETDrawCommands.hpp"

enum class EDrawCmdEventType {
    None = 0,
    Reorder = 1 << 1,
    UpdateVertexData = 1 << 2
};

class DrawCmdSlice {
public:

    DrawCmdSlice() :
        queue(nullptr),
        size(0),
        startIdx(0),
        endIdx(0) {}

    DrawCmdSlice(DrawCmd** cmdsQueuePtr, size_t cmdsQueueSize) :
        queue(cmdsQueuePtr),
        size(cmdsQueueSize),
        startIdx(0),
        endIdx(0) {}

    bool operator<(const DrawCmdSlice& other) {
        if(other.empty()) {
            return !empty();
        }
        if(empty()) {
            return false;
        }
        return queue[startIdx]->zIndex < other.queue[other.startIdx]->zIndex;
    }

    bool empty() const {
        if(!queue) {
            return true;
        }
        if(startIdx == size) {
            return true;
        }
        return false;
    }

    void makeMaxSlice() {
        endIdx = size;
    }

    void tryToGrowUpTo(const DrawCmdSlice& other) {
        int refZIndex = other.queue[other.startIdx]->zIndex;
        for(size_t i = endIdx; i < size; ++i) {
            if(queue[i]->zIndex <= refZIndex) {
                ++endIdx;
            } else {
                break;
            }
        }
    }

    void advance() {
        if(empty()) {
            return;
        }
        startIdx = endIdx;
        if(startIdx >= size) {
            return;
        }
        int refZIndex = queue[startIdx]->zIndex;
        for(size_t i = startIdx; i < size; ++i) {
            if(queue[i]->zIndex == refZIndex) {
                endIdx += 1;
            }
        }
    }

public:

    DrawCmd** queue;
    size_t size;
    size_t startIdx;
    size_t endIdx;
};

namespace DrawCmdUtils {

template<typename T>
size_t SortCmdDrawQueue(std::vector<T*>& queue) {
    if(queue.empty()) {
        return 0u;
    }

    size_t i = 0;
    size_t j = queue.size() - 1;

    while(i <= j) {
        if(T::IsVisible(*queue[i])) {
            ++i;
        } else {
            if(i != j) {
                std::swap(queue[i], queue[j]);
                --j;
            } else {
                break;
            }
        }
    }

    if(i > 1) {
        std::sort(queue.begin(), queue.begin() + i, [](const T* first, const T* second){
            return first->zIndex < second->zIndex;
        });
    }

    return i;
}

template<typename T>
void RemoveCmdFromQueueAndPool(std::vector<T*>& queue, Memory::ObjectPool<T>& pool, DrawCmd* cmd) {
    if(!cmd) {
        return;
    }

    auto it = std::find(queue.begin(), queue.end(), cmd);
    if(it != queue.end()) {
        std::swap(*it, queue.back());
        queue.pop_back();
    } else {
        return;
    }

    pool.recycle(cmd);
}

} // namespace DrawCmdUtils

class BaseDrawCommandExectuor {
public:

    virtual ~BaseDrawCommandExectuor() = default;

    virtual bool init() = 0;
    virtual void deinit() = 0;
    virtual void draw(RenderState& renderState, DrawCmdSlice& slice) = 0;

    virtual void preDraw(RenderState& renderState) = 0;
    virtual DrawCmd* createCmd() = 0;
    virtual void registerCmdForDraw(DrawCmd* cmd) = 0;
    virtual void destroyCmd(DrawCmd* cmd) = 0;
    virtual DrawCmdSlice getCmdSlice() = 0;

    void addEvent(EDrawCmdEventType eventType) {
        events = Core::EnumFlagsBitXOR(events, eventType);
    }

    void clearEvents() {
        events = EDrawCmdEventType::None;
    }

protected:

    EDrawCmdEventType events{EDrawCmdEventType::None};
    size_t visibleCount{0};
};

template<typename T>
class DrawCommandExecutor : public BaseDrawCommandExectuor {
public:

    virtual ~DrawCommandExecutor() = default;

    DrawCmd* createCmd() override {
        return reinterpret_cast<DrawCmd*>(pool.create());
    }

    void preDraw(RenderState& renderState) override {
        if(Core::EnumFlagsBitAND(events, EDrawCmdEventType::Reorder)) {
            visibleCount = DrawCmdUtils::SortCmdDrawQueue(queue);
            if(visibleCount > 0) {
                addEvent(EDrawCmdEventType::UpdateVertexData);
            }
        }
        if(visibleCount == 0) {
            clearEvents();
        }
    }

    void registerCmdForDraw(DrawCmd* cmd) override {
        queue.push_back(static_cast<T*>(cmd));
        if(T::IsVisible(*static_cast<T*>(cmd))) {
            addEvent(EDrawCmdEventType::Reorder);
        }
    }

    void destroyCmd(DrawCmd* cmd) override {
        DrawCmdUtils::RemoveCmdFromQueueAndPool(queue, pool, cmd);
        addEvent(EDrawCmdEventType::Reorder);
    }

    DrawCmdSlice getCmdSlice() override {
        return DrawCmdSlice(reinterpret_cast<DrawCmd**>(&queue[0]), visibleCount);
    }

protected:

    Memory::ObjectPool<T> pool;
    std::vector<T*> queue;
};

#endif /* __DRAW_COMMAND_EXECUTOR_HPP__ */