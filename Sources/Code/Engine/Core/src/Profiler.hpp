#ifndef __PROFILER_HPP__
#define __PROFILER_HPP__

#include <thread>
#include <vector>

class Profiler {
public:

    Profiler();
    ~Profiler();

    void enterSection(const char* sectionName);
    void leaveSection();

private:

    struct SectionNode {
        const char* name;
    };

    struct ThreadStack {
        std::thread::id id;
        std::vector<SectionNode> stack;
    };

private:

    std::thread::id mainThreadId;
    std::vector<ThreadStack> threadStacks;
};

#endif /* __PROFILER_HPP__ */