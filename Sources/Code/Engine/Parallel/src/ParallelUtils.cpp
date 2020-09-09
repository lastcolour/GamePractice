#include "ParallelUtils.hpp"

#include <thread>
#include <chrono>

namespace {

const int IDLE_STEP_MS = 5;

} // namespace

namespace Parallel {

void ThreadIdle() {
    std::this_thread::sleep_for(std::chrono::milliseconds(IDLE_STEP_MS));
}

} // namespace Parallel