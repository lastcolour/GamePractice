#include "Core/MainMemoryAllocator.hpp"
#include "Parallel/TasksRunner.hpp"
#include "Reflect/ClassInfoManager.hpp"

#include <cassert>

namespace Core {

GlobalEnvironment* GlobalEnvironment::ENV = nullptr;

GlobalEnvironment::GlobalEnvironment() {
    if(ENV) {
        assert(false && "Global ENV already exists");
    } else {
        ENV = this;
    }
    memoryAllocator.reset(new Memory::MainMemoryAllocator);
    etSystem.reset(new ET::ETSystem);
    taskRunner.reset(new TasksRunner);
    globalData.reset(new GlobalData);
    classInfoManager.reset(new Reflect::ClassInfoManager);
}

GlobalEnvironment::~GlobalEnvironment() {
    globalData.reset();
    classInfoManager.reset();
    if(ENV == this) {
        ENV = nullptr;
    }
}

} // namepsace Core