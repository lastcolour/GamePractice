#include "Game/Logics/BronKerbosch.hpp"
#include "Core/TimePoint.hpp"

#include <cassert>
#include <iostream>

AdjacencyMatrix::AdjacencyMatrix() :
    vertNum(0),
    edgeNum(0) {
}

AdjacencyMatrix::AdjacencyMatrix(int numVertecies) :
    AdjacencyMatrix() {

    reset(numVertecies);
}

AdjacencyMatrix::~AdjacencyMatrix() {
}

void AdjacencyMatrix::reset(int numVertecies) {
    edgeNum = 0;
    vertNum = numVertecies;

    assert(vertNum >= 0 && "Invalid amount of verticies");

    data.resize(vertNum * vertNum);
    std::fill(data.begin(), data.end(), 0);

    for(int i = 0; i < vertNum; ++i) {
        setVertexCost(i, 1);
    }
}

void AdjacencyMatrix::addEdge(int a, int b) {
    assert(a >= 0 && a < vertNum && "Vertex id out of range");
    assert(b >= 0 && b < vertNum && "Vertex id out of range");

    if(a > b) {
        std::swap(a, b);
    } else if(a == b) {
        return;
    }

    auto idx = a * vertNum + b;

    if(idx >= data.size()) {
        std::cout << "Error = " << idx << ", a = " << a << ", b = " << b << std::endl;
    }

    if(!data[idx]) {
        data[idx] = 1;
        ++edgeNum;
    }
}

bool AdjacencyMatrix::hasEdge(int a, int b) const {
    assert(a >= 0 && a < vertNum && "Vertex id out of range");
    assert(b >= 0 && b < vertNum && "Vertex id out of range");

    if(a == b) {
        return false;
    } else if(a > b) {
        std::swap(a, b);
    }

    auto idx = a * vertNum + b;
    return data[idx];
}

int AdjacencyMatrix::vertexCount() const {
    return vertNum;
}

int AdjacencyMatrix::edgeCount() const {
    return edgeNum;
}

int AdjacencyMatrix::getVertexCost(int a) const {
    assert(a >= 0 && a < vertNum && "Vertex id out of range");
    auto idx = a * vertNum + a;
    return data[idx];
}

void AdjacencyMatrix::setVertexCost(int a, int cost) {
    assert(a >= 0 && a < vertNum && "Vertex id out of range");
    assert(cost > 0 && "Invalid vertex cost");

    auto idx = a * vertNum + a;
    data[idx] = cost;
}

namespace {

struct BK_State {
    std::vector<int> R;
    std::vector<int> P;
    std::vector<int> X;
};


void removeByValue(std::vector<int>& vec, int x) {
    if(vec.empty()) {
        return;
    }
    vec.erase(std::remove(vec.begin(), vec.end(), x), vec.end());
}

template<typename F>
void removeByPredicate(std::vector<int>& vec, F pred) {
    if(vec.empty()) {
        return;
    }
    vec.erase(std::remove_if(vec.begin(), vec.end(), pred), vec.end());
}

} // namespace

struct BronKerboschCache {
    std::vector<BK_State> states;
};

BronKerboschRequest::BronKerboschRequest() :
    cache(nullptr),
    currStopIterCount(0),
    stopIterCount(1000000),
    maxCost(0),
    iterCount(0),
    maxDepth(0),
    duration(0.f),
    stopBecauseLimit(false) {
}

BronKerboschRequest::~BronKerboschRequest() {
    if(cache) {
        delete cache;
        cache = nullptr;
    }
}

namespace {

void updateResult(BronKerboschRequest& req, std::vector<int>& clique) {
    if(clique.empty()) {
        return;
    }
    int totalCost = 0;
    for(auto& i : clique) {
        totalCost += req.adjMat.getVertexCost(i);
    }
    if(totalCost > req.maxCost) {
        req.maxCost = totalCost;
        req.result = std::move(clique);
        req.currStopIterCount = 0;
    }
}

bool shouldStopByMaxIter(BronKerboschRequest& req) {
    if(req.stopIterCount == -1) {
        return false;
    }
    if(req.currStopIterCount >= req.stopIterCount) {
        req.stopBecauseLimit = true;
        return true;
    }
    return false;
}

void BronKerbosch_Impl(int depth, BronKerboschRequest& req) {
    if(shouldStopByMaxIter(req)) {
        return;
    }

    req.iterCount += 1;
    req.currStopIterCount += 1;
    req.maxDepth = std::max(req.maxDepth, depth);

    auto state = &req.cache->states[depth];
    if(state->P.empty() && state->X.empty()) {
        updateResult(req, state->R);
    } else {
        auto P_copy = state->P;
        for(auto& v : P_copy) {
            if(req.cache->states.size() <= (depth + 1)) {
                req.cache->states.emplace_back();
                state = &req.cache->states[depth];
            }
            auto newState = &req.cache->states[depth + 1];
            *newState = *state;

            newState->R.push_back(v);

            removeByPredicate(newState->X, [adjMat=&req.adjMat, v](int u){
                return !adjMat->hasEdge(u, v);
            });

            removeByPredicate(newState->P, [adjMat=&req.adjMat, v](int u){
                return !adjMat->hasEdge(u, v);
            });

            BronKerbosch_Impl(depth + 1, req);

            if(shouldStopByMaxIter(req)) {
                return;
            }
 
            state = &req.cache->states[depth];

            removeByValue(state->P, v);
            state->X.push_back(v);
        }
    }
}

} // namespace

void BronKerbosch(BronKerboschRequest& req) {
    auto startT = TimePoint::GetNowTime();

    req.result.clear();
    req.currStopIterCount = 0;
    req.maxCost = 0;
    req.iterCount = 0;
    req.maxDepth = 0;
    req.duration = 0.f;
    req.stopBecauseLimit = false;

    if(req.adjMat.vertexCount() == 0) {
        return;
    }
    if(!req.cache) {
        req.cache = new BronKerboschCache;
        req.cache->states.emplace_back();
    }

    req.cache->states[0].P.clear();
    req.cache->states[0].R.clear();
    req.cache->states[0].X.clear();

    auto& state = req.cache->states[0];
    for(int i = 0, sz = req.adjMat.vertexCount(); i < sz; ++i) {
        state.P.push_back(i);
    }

    std::sort(state.P.begin(), state.P.end(), [&req](int a, int b){
        return req.adjMat.getVertexCost(a) > req.adjMat.getVertexCost(b);
    });

    BronKerbosch_Impl(0, req);

    auto endT = TimePoint::GetNowTime();
    req.duration = endT.getMiliSecElapsedFrom(startT);
}